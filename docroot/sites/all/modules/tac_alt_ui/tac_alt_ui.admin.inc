<?php

/**
 * @file
 * Admin functions for simplified UI for the taxonomy access control module.
 */

/**
 * Implements hook_form_FORM_ID_alter().
 */
function _tac_alt_ui_form_field_ui_field_edit_form_alter(&$form, &$form_state) {
  // Help the administrator avoid creating a multivalued term reference field to
  // an access controlled vocabulary. tac_alt_ui_field_update_forbid() prevents
  // it, but here we guide the UI to avoid triggering those errors.
  if ($form['#field']['type'] == 'taxonomy_term_reference') {
    foreach (tac_alt_ui_get_enabled_vocabs() as $i => $vocab) {
      if ($form['field']['cardinality']['#default_value'] != 1) {
        // If the field is already multivalued and not using this access
        // controlled vocabulary, then remove it as an option. Require the
        // administrator to first save the field as single-valued (which might
        // entail Field UI warning about data loss), and then revisit this form
        // to switch to an access controlled vocabulary. If the field is already
        // using this vocabulary, don't remove it as an option; allow the
        // administrator to decide how and when to bring the site back into
        // compatibility with this module's requirements.
        if ($form['field']['settings']['allowed_values'][0]['vocabulary']['#default_value'] != $vocab && isset($form['field']['settings']['allowed_values'][0]['vocabulary']['#options'][$vocab])) {
          unset($form['field']['settings']['allowed_values'][0]['vocabulary']['#options'][$vocab]);
          if (empty($updated_description)) {
            $form['field']['settings']['allowed_values'][0]['vocabulary']['#description'] .= ' ' . t('Some vocabularies do not appear in this list, because they cannot be used for a multivalued field. To use one of those vocabularies, first save this form with %cardinality changed to %value.', array('%cardinality' => $form['field']['cardinality']['#title'], '%value' => 1));
            $updated_description = TRUE;
          }
        }
      }
      else {
        // If the field is currently single-valued, then hide the cardinality
        // setting when an access controlled vocabulary is selected.
        // - We use 'visible' and '!value' rather than 'invisible' and 'value',
        //   because #states ANDs rather than ORs multiple conditions.
        // - We append an additional dummy selector for an element that is not
        //   in the DOM, and therefore, has no impact on what is selected, so
        //   that when there are multiple access controlled vocabularies, each
        //   condition can have a unique index within #states.
        // - We disable in addition to hiding, to prevent the browser from
        //   submitting a value, allowing Form API to use the default value.
        //
        // @todo When http://drupal.org/node/735528 is fixed, this can be
        //   cleaned up to use it.
        $form['field']['cardinality']['#states']['visible'][':input[name="field[settings][allowed_values][0][vocabulary]"], #tac-alt-ui-null-' . $i] = array('!value' => $vocab);
        $form['field']['cardinality']['#states']['enabled'][':input[name="field[settings][allowed_values][0][vocabulary]"], #tac-alt-ui-null-' . $i] = array('!value' => $vocab);
      }
    }
  }
}

/**
 * Implement hook_form_FORM_ID_alter().
 */
function _tac_alt_ui_form_taxonomy_form_term_alter(&$form, &$form_state) {
  // Don't do anything on term deletion page.
  if (isset($form_state['confirm_delete'])) {
    return;
  }
  $enabled_vocabs = tac_alt_ui_get_enabled_vocabs();
  if (!in_array($form['#vocabulary']->machine_name, $enabled_vocabs)) {
    return;
  }
  // Keep TAC-enabled vocabularies flat.
  $form['relations']['parent']['#access'] = FALSE;
  $form['relations']['parent']['#value'] = 0;

  // Add the permission matrix inside a collapsible fieldset. Other modules
  // might add more elements to the fieldset.
  $form['taxonomy_access_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content access permissions'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 11,  // Just below $form['relations'].
  );
  $form['taxonomy_access_fieldset']['taxonomy_access'] = array(
    '#tree' => TRUE,
    '#access' => user_access('administer permissions'),
    '#type' => 'item',
    '#title' => t('Select the roles and permissions that will be applied to any content assigned with this term.'),
  );
  $term = (object) $form['#term'];
  $form['taxonomy_access_fieldset']['taxonomy_access'] += tac_alt_ui_term_permissions_matrix($term);
  $form['#submit'][] = 'tac_alt_ui_form_taxonomy_form_term_submit';
}

/**
 * Added form submit function for taxonomy_form_term().
 */
function tac_alt_ui_form_taxonomy_form_term_submit(&$form, &$form_state) {
  if ($form_state['triggering_element']['#value'] != t('Delete')) {
    $grant_rows = array();
    foreach ($form_state['values']['taxonomy_access']['checkboxes'] as $rid => $grants) {
      $row = new stdClass();
      $row->rid = $rid;
      $row->tid = $form_state['values']['tid'];
      foreach ($grants as $grant => $value) {
        switch ($grant) {
          case 'grant_create':
          case 'grant_list':
            $row->{$grant} = ($value ? TAXONOMY_ACCESS_TERM_ALLOW : TAXONOMY_ACCESS_TERM_DENY);
            break;
          default:
            $row->{$grant} = ($value ? TAXONOMY_ACCESS_NODE_ALLOW : TAXONOMY_ACCESS_NODE_DENY);
            break;
        }
      }
      $grant_rows[] = $row;
    }
    taxonomy_access_set_term_grants($grant_rows);
  }
}

/**
 * Returns a TAC permissions matrix that can be inserted into a form.
 *
 * The matrix's display is based on that of Drupal's default permissions page.
 *
 * Note that this matrix must be accompanied by an appropriate submit handler
 * (attached to the top level of the form) in order for the grants in it
 * to actually be saved.
 *
 * @param $term
 *   The term whose permissions will be displayed in the matrix.
 *
 * @return
 *   A form array defining the permissions matrix.
 *
 * @see user_admin_permissions()
 * @see _field_permissions_field_settings_form_submit()
 */
function tac_alt_ui_term_permissions_matrix($term) {
  // This function primarily contains a simplified version of the code from
  // user_admin_permissions().
  $form['#theme'] = 'user_admin_permissions';
  $options = array();
  $status = array();

  // Retrieve all role names for use in the submit handler.
  $role_names = user_roles();
  $form['role_names'] = array(
    '#type' => 'value',
    '#value' => $role_names,
  );
  // Retrieve the term grants for each role, by role ID.
  $grants = array();
  if (empty($term->tid)) {
    // For new terms, use the vocabulary defaults.
    $vocab = taxonomy_vocabulary_machine_name_load($term->vocabulary_machine_name);
    $grants = db_query('SELECT * FROM {taxonomy_access_default} WHERE vid = :vid', array(':vid' => $vocab->vid))->fetchAllAssoc('rid', PDO::FETCH_ASSOC);
  }
  else {
    // For existing terms, use the term-specific grants if they exist. If this
    // is a term that existed before the vocabulary was TAC-enabled, there will
    // be no grants, so fall back to the vocabulary defaults.
    $grants = db_query('SELECT * FROM {taxonomy_access_term} WHERE tid = :tid', array(':tid' => $term->tid))->fetchAllAssoc('rid', PDO::FETCH_ASSOC);
    if (empty($grants)) {
      $grants = db_query('SELECT * FROM {taxonomy_access_default} WHERE vid = :vid', array(':vid' => $term->vid))->fetchAllAssoc('rid', PDO::FETCH_ASSOC);
    }
  }

  // Go through each taxonomy access permission we will display.
  foreach (tac_alt_ui_permissions_list() as $permission => $info) {
    // Display the name of the permission as a form item.
    $form['permission'][$permission] = array(
      '#type' => 'item',
      '#markup' => $info['title'],
    );
    // Save it to be displayed as one of the role checkboxes.
    $options[$permission] = '';

    foreach ($role_names as $rid => $name) {
      if (isset($grants[$rid][$permission]) && ($grants[$rid][$permission] == TAXONOMY_ACCESS_NODE_ALLOW || $grants[$rid][$permission] == TAXONOMY_ACCESS_TERM_ALLOW)) {
        $status[$rid][] = $permission;
      }
    }
  }

  // Build the checkboxes for each role.
  foreach ($role_names as $rid => $name) {
    $form['checkboxes'][$rid] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => isset($status[$rid]) ? $status[$rid] : array(),
      '#attributes' => array('class' => array('rid-' . $rid)),
    );
    $form['role_names'][$rid] = array(
      '#markup' => check_plain($name),
      '#tree' => TRUE,
    );
  }
  // Attach the default permissions page JavaScript and our CSS.
  $form['#attached']['js'][] = drupal_get_path('module', 'user') . '/user.permissions.js';
  $form['#attached']['css'][] = drupal_get_path('module', 'tac_alt_ui') . '/tac_alt_ui.admin.css';

  return $form;
}

/**
 * Obtain the list of field permissions.
 *
 * @param $field_label
 *   The human readable name of the field to use when constructing permission
 *   names. Usually this will be derived from one or more of the field instance
 *   labels.
 */
function tac_alt_ui_permissions_list() {
  return array(
    'grant_view' => array(
      'title' => t('View content with this term'),
      'description' => t(''),
    ),
    'grant_update' => array(
      'title' => t('Edit any content with this term'),
      'description' => t('Allows editing any content, overriding normal content permissions.'),
      'restrict access' => TRUE,
    ),
    'grant_delete' => array(
      'title' => t('Delete any content with this term'),
      'description' => t(''),
      'restrict access' => TRUE,
    ),
    'grant_create' => array(
      'title' => t('Add this term to content'),
      'description' => t('Allows the user to add this term to content during creation or editing.'),
    ),
    'grant_list' => array(
      'title' => t('View this term in rendered content'),
      'description' => t('Allows the user to see the term itself when viewing content that has the term.'),
    ),
  );
}

/**
 * Helper function to mark vocabularies as being used for access control.
 */
function _tac_alt_ui_form_taxonomy_overview_vocabularies_alter(&$form, &$form_state) {
  $enabled_vocabs = tac_alt_ui_get_enabled_vocabs();
  foreach (element_children($form) as $vid) {
    if (isset($form[$vid]['#vocabulary'])) {
      // Mark each TAC-enabled vocabulary as "access control"...
      $vocabulary = $form[$vid]['#vocabulary'];
      // ...but avoid being Captain Obvious.
      if ($vocabulary->name == t('Access control')) {
        continue;
      }
      $access_control = in_array($vocabulary->machine_name, $enabled_vocabs);
      if ($access_control) {
        $form[$vid]['name']['#markup'] .= ' ' . t('(access control enabled)');
      }
    }
  }
}

/**
 * Helper function to add a vocabulary enable/disable checkbox.
 */
function _tac_alt_ui_form_taxonomy_form_vocabulary_alter(&$form, &$form_state) {
  $vocabulary = $form_state['vocabulary'];
  $enabled = tac_alt_ui_vocabulary_is_enabled($vocabulary->machine_name);
  $form['tac_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable terms to manage content access.'),
    '#default_value' => $enabled,
    '#disabled' => FALSE,
  );
  if ($enabled) {
    // If this vocabulary already has terms, disabling it will remove all those
    // term grants. Add a warning to that effect.
    $existing_terms = db_query("SELECT COUNT(1) FROM {taxonomy_term_data} ttd JOIN {taxonomy_access_term} tat ON ttd.tid = tat.tid WHERE ttd.vid = :vid", array(':vid' => $vocabulary->vid))->fetchField();
    if ($existing_terms) {
      $form['warning'] = array(
        '#type' => 'item',
        '#markup' => '<strong>' . t('Disabling this vocabulary will also remove access control from all its existing terms.') . '</strong>',
        '#states' => array(
          'visible' => array(
            ':input[name="tac_enable"]' => array('checked' => FALSE),
          ),
        ),
      );
    }
    $form['#submit'][] = 'tac_alt_ui_form_taxonomy_form_vocabulary_submit';
  }
  else {
    // Make sure to disable the checkbox in situations where enabling the
    // vocabulary would turn off the simple UI.
    $eligibility = tac_alt_ui_get_eligibility_status($vocabulary);
    if ($eligibility === TRUE) {
      // The vocabulary is eligible for TAC. Let the admin change its status.
      $form['#submit'][] = 'tac_alt_ui_form_taxonomy_form_vocabulary_submit';
    }
    else {
      // The vocabulary is ineligible for some reason. Provide an explanation.
      $form['tac_enable']['#disabled'] = TRUE;
      $form['tac_enable']['#description'] = t('This vocabulary cannot be used for access control: !explanation', array('!explanation' => $eligibility));
    }

  }
}

/**
 * Form submit callback for the taxonomy vocabulary form.
 */
function tac_alt_ui_form_taxonomy_form_vocabulary_submit(&$form, &$form_state) {
  // TAC-enable or disable the vocabulary.
  $enable = $form_state['values']['tac_enable'];
  if ($enable) {
    tac_alt_ui_enable_vocabulary($form_state['values']['machine_name']);
  }
  else {
    tac_alt_ui_disable_vocabulary($form_state['values']['machine_name']);
  }
}
