<?php

/**
 * @file
 *   Form builder implementation for node.module.
 */

/**
 * Main page for editing a content type form.
 *
 * @param $type
 *   The content type object being edited.
 */
function node_form_edit($type) {
  module_load_include('inc', 'form_builder', 'includes/form_builder.admin');

  // TODO: These should be auto-loaded as necessary.
  drupal_add_js('misc/autocomplete.js');
  drupal_add_js('misc/collapse.js');
  drupal_add_js('misc/tableheader.js');
  drupal_add_js('misc/textarea.js');

  drupal_set_message(t('This is a form preview. Click on a field to edit its properties.'), 'warning');

  $output = array();
  $output[] = form_builder_interface('node', $type->type);
  $output[] = drupal_get_form('node_form_builder_save_form', $type);
  return $output;
}

/**
 * Form for saving Form Builder changes.
 */
function node_form_builder_save_form($form_state, $type, $review = FALSE) {
  module_load_include('inc', 'form_builder', 'includes/form_builder.api');
  module_load_include('inc', 'form_builder', 'includes/form_builder.cache');

  $form = array();

  if ($review) {
    // Allow modules to review dangerous changes before continuing.
    $differences = form_builder_cache_difference('node', $type->type);
    if ($differences !== FALSE) {
      $form['warnings'] = array(
        '#tree' => TRUE,
      );

      if (empty($form_state['post'])) {
        drupal_set_message(t('Please review your changes before saving.'));
      }

      $changed_fields = array();
      foreach ($differences as $difference) {
        $field_name = isset($difference['original']['#title']) ? $difference['original']['#title'] : $difference['original']['#form_builder']['element_id'];
        if (empty($difference['original'])) {
          $field_name = isset($difference['modified']['#title']) ? $difference['modified']['#title'] : $difference['modified']['#form_builder']['element_id'];
          $changed_fields[] = $field_name . ' - <strong>' . t('Added') . '</strong>';
        }
        elseif (empty($difference['modified'])) {
          $changed_fields[] = $field_name . ' - <strong>' . t('Removed') . '</strong>';
        }
        else {
          $changed_fields[] = $field_name . ' - <strong>' . t('Modified') . '</strong>';
        }
      }

      $form['changed'] = array(
        '#type' => 'markup',
        '#value' => theme('item_list', $changed_fields),
      );

      foreach ($differences as $difference) {
        $element_id = $difference['original']['#form_builder']['element_id'];
        $correction_form = module_invoke_all('form_builder_correction_form', $form_state, 'node', $type->type, $element_id, $difference['original'], $difference['modified']);
        if (!empty($correction_form)) {
          $form['warnings'][$element_id] = $correction_form;
        }
      }

      $form['offline'] = array(
        '#title' => t('Site status'),
        '#type' => 'radios',
        '#options' => array(
          '0' => t('Leave site online'),
          '1' => t('Temporarily take site offline'),
        ),
        '#default_value' => '0',
        '#description' => t('For large sets of changes, it is recommended that you temporarily disable access to your site while data is being saved. After changes are complete, the site will be turned back online. You can change the site status manually be visiting <a href="!url">Site maintenance</a>.', array('!url' => url('admin/settings/site-maintenance'))),
        '#access' => variable_get('site_offline', '0') == '0',
      );

      $form['buttons']['cancel'] = array(
        '#type' => 'markup',
        '#value' => l('Cancel', 'admin/build/node-type/' . str_replace('_', '-', $type->type) . '/fields'),
      );

      $form['reviewed'] = array(
        '#type' => 'value',
        '#value' => TRUE,
      );
    }
    else {
      drupal_not_found();
      exit;
    }
  }

  $form['type'] = array(
    '#type' => 'value',
    '#value' => $type,
  );

  $form['buttons']['#weight'] = 100;

  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => -1,
  );

  return $form;
}

/**
 * Submit handler for saving changes to the node form.
 */
function node_form_builder_save_form_submit(&$form, &$form_state) {
  module_load_include('inc', 'form_builder', 'includes/form_builder.api');
  module_load_include('inc', 'form_builder', 'includes/form_builder.cache');
  $type = $form_state['values']['type'];

  if (!isset($form_state['values']['reviewed'])) {
    $form_state['redirect'] = 'admin/build/node-type/' . str_replace('_', '-', $type->type) .'/fields/confirm';
    return;
  }

  // Disable access to the site.
  if ($form_state['values']['offline'] == '1') {
    variable_set('site_offline', '1');
  }

  $node_form = form_builder_cache_load('node', $type->type);
  form_builder_save_form($node_form, 'node', $type->type);

  // Re-enable access to the site.
  if ($form_state['values']['offline'] == '1') {
    variable_set('site_offline', '0');
  }

  drupal_set_message(t('The changes to the %type content type form have been saved.', array('%type' => $type->name)));
  $form_state['redirect'] = 'admin/build/node-type/' . str_replace('_', '-', $type->type);

}

/**
 * Implementation of hook_form_builder_types().
 */
function node_form_builder_types() {
  global $user;

  module_load_include('inc', 'node', 'node.pages');

  $fields = array();

  $node = array(
    'uid' => $user->uid,
    'created' => 280321200, // 19 Nov 1978 05:00:00.
    'date' => format_date(280321200, 'custom', 'Y-m-d H:i:s O'),
    'body' => NULL,
    'title' => NULL,
    'format' => NULL,
  );
  $settings = module_invoke('node', 'node_form_default_settings');

  $node['status'] = $settings['options']['status'];
  $node['promote'] = $settings['options']['promote'];
  $node['sticky'] = $settings['options']['sticky'];

  $node = (object) $node;

  $title_form = node_title_field($node, $settings['title']);
  $title_form['#weight'] = $settings['title']['weight'];
  $fields['title'] = array(
    'title' => t('Title'),
    'properties' => array(
      'title',
    ),
    'default' => $title_form,
    'unique' => TRUE,
    'removable' => FALSE,
    'palette_group' => 'special',
  );

  /*$body_form = node_body_field($node, $settings['body_field']['label'], $settings['body_field']['min_word_count']);
  $body_form['#weight'] = $settings['body_field']['weight'];
  $body_form['#body_settings'] = $settings['body_field'];
  $fields['body_field'] = array(
    'title' => t('Body'),
    'properties' => array(
      'body_settings',
    ),
    'default' => $body_form,
    'unique' => TRUE,
    'palette_group' => 'special',
  );*/

  $revision_fieldset = node_revision_fieldset($node, $settings['revision_information']);
  $revision_fieldset['#weight'] = $settings['revision_information']['weight'];
  $revision_fieldset['#revision_settings'] = $settings['revision_information'];
  $fields['revision_information'] = array(
    'title' => t('Revisions'),
    'properties' => array(
      'collapsible',
      'collapsed',
      'revision_settings',
    ),
    'default' => $revision_fieldset,
    'unique' => TRUE,
    'palette_group' => 'special',
  );

  $author_fieldset = node_author_fieldset($node, $settings['author']);
  $author_fieldset['#author_settings'] = $settings['author'];
  $author_fieldset['#weight'] = $settings['author']['weight'];
  $author_fieldset['#form_builder']['removable'] = FALSE;
  $fields['author'] = array(
    'title' => t('Authoring'),
    'properties' => array(
      'collapsible',
      'collapsed',
    ),
    'default' => $author_fieldset,
    'unique' => TRUE,
    'removable' => FALSE,
    'palette_group' => 'special',
  );

  $options_fieldset = node_options_fieldset($node, $settings['options']);
  $options_fieldset['#options_settings'] = $settings['options'];
  $options_fieldset['#weight'] = $settings['options']['weight'];
  $options_fieldset['#form_builder']['removable'] = FALSE;
  $fields['options'] = array(
    'title' => t('Options'),
    'properties' => array(
      'collapsible',
      'collapsed',
      'options_settings',
    ),
    'default' => $options_fieldset,
    'unique' => TRUE,
    'removable' => FALSE,
    'palette_group' => 'special',
  );

  // Generic field for handling weights.
  $fields['generic_field'] = array(
    'title' => t('Generic field'),
    'properties' => array(),
    'removable' => FALSE,
    'configurable' => FALSE,
  );

  return array(
    'node' => $fields,
  );
}

/**
 * Implementation of hook_form_builder_properties().
 */
function node_form_builder_properties($form_type) {
  if ($form_type == 'node') {
    return array(
      'body_settings' => array(
        'form' => 'node_body_settings_form',
        'submit' => array('node_body_settings_form_submit'),
      ),
      'revision_settings' => array(
        'form' => 'node_revision_settings_form',
      ),
      'options_settings' => array(
        'form' => 'node_options_settings_form',
      ),
    );
  }
}

/**
 * Implementation of hook_form_builder_preview_alter().
 *
 * Update the custom node fields according to the changed node type settings.
 */
function node_form_builder_preview_alter(&$element, $form_type, $form_id) {
  if ($form_type == 'node') {
    switch ($element['#form_builder']['element_id']) {
      case 'title':
        // No node title settings yet.
        break;
      case 'body_field':
        $element['body']['#title'] = check_plain($element['#body_settings']['label']);
        $element['body']['#required'] = $element['#body_settings']['min_word_count'] > 0;
        break;
      case 'revision_information':
        $element['revision']['#default_value'] = $element['#revision_settings']['revision'];
        break;
      case 'author':
        // No node author settings yet.
        break;
      case 'options':
        $element['status']['#default_value'] = $element['#options_settings']['status'];
        $element['promote']['#default_value'] = $element['#options_settings']['promote'];
        $element['sticky']['#default_value'] = $element['#options_settings']['sticky'];
        break;
    }
  }
}

/**
 * Configuration form for the "body_settings" property.
 */
function node_body_settings_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['label'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#parents' => array('body_settings', 'label'),
    '#default_value' => $element['#body_settings']['label'],
    '#required' => TRUE,
    '#weight' => -10,
  );

  $form['min_word_count'] = array(
    '#form_builder' => array('property_group' => 'validation'),
    // Manually set parents since we need this to be in the validation group.
    '#parents' => array('body_settings', 'min_word_count'),
    '#type' => 'select',
    '#title' => t('Minimum number of words'),
    '#default_value' => $element['#body_settings']['min_word_count'],
    '#options' => drupal_map_assoc(array(0, 10, 25, 50, 75, 100, 125, 150, 175, 200)),
    '#description' => t('The minimum number of words for the body field to be considered valid for this content type. This can be useful to rule out submissions that do not meet the site\'s standards, such as short test posts.')
  );

  return $form;
}

/**
 * Configuration form for the "revision_settings" property.
 */
function node_revision_settings_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['revision_settings']['revision'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create new revision by default'),
    '#parents' => array('revision_settings', 'revision'),
    '#default_value' => $element['#revision_settings']['revision'],
  );

  return $form;
}

/**
 * Configuration form for the "options_settings" property.
 */
function node_options_settings_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Published by default'),
    '#parents' => array('options_settings', 'status'),
    '#default_value' => $element['#options_settings']['status'],
  );

  $form['promote'] = array(
    '#type' => 'checkbox',
    '#title' => t('Promoted by default'),
    '#parents' => array('options_settings', 'promote'),
    '#default_value' => $element['#options_settings']['promote'],
  );

  $form['sticky'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sticky by default'),
    '#parents' => array('options_settings', 'sticky'),
    '#default_value' => $element['#options_settings']['sticky'],
  );

  return $form;
}

/**
 * Implementation of hook_form_builder_load().
 */
function node_form_builder_load($form_type, $form_id) {
  global $user;

  module_load_include('inc', 'node', 'node.pages');

  if ($form_type == 'node') {
    $form_state = array('submitted' => FALSE);
    $node_type = $form_id;

    $node = array(
      'uid' => $user->uid,
      'created' => 280321200, // 19 Nov 1978 05:00:00.
      'date' => format_date(280321200, 'custom', 'Y-m-d H:i:s O'),
      'name' => (isset($user->name) ? $user->name : ''),
      'type' => $node_type,
      'language' => '',
    );
    $form = node_form($form_state, $node);

    // Setup generic weight handling.
    drupal_prepare_form($form_id .'_node_form', $form, $form_state);
    $form['#node_generic_fields'] = array();
    foreach (element_children($form) as $key) {
      $element = $form[$key];
      if (!isset($element['#form_builder']) && (!empty($element['#tree']) || (isset($element['#type']) && !in_array($element['#type'], array('value', 'hidden', 'markup', 'token', 'button', 'submit', 'vertical_tabs', 'horizontal_tabs'))))) {
        $form[$key]['#form_builder'] = array(
          'element_id' => $key,
          'element_type' => 'generic_field',
        );
        $form['#node_generic_fields'][] = $key;
      }
    }

    if (isset($form['title'])) {
      $title_settings = node_get_form_settings($node_type, 'title');
      $form['title']['#title_settings'] = $title_settings;
      $form['title']['#weight'] = $title_settings['weight'];
      $form['title']['#form_builder'] = array(
        'element_id' => 'title',
        'element_type' => 'title',
      );
    }

    if (isset($form['body_field'])) {
      $body_settings = node_get_form_settings($node_type, 'body_field');
      $form['body_field']['#body_settings'] = $body_settings;
      $form['body_field']['#weight'] = $body_settings['weight'];
      $form['body_field']['#form_builder'] = array(
        'element_id' => 'body_field',
        'element_type' => 'body_field',
      );
    }

    $revision_settings = node_get_form_settings($node_type, 'revision_information');
    if ($revision_settings['enabled']) {
      $form['revision_information'] = node_revision_fieldset($form['#node'], $revision_settings);
      $form['revision_information']['#revision_settings'] = $revision_settings;
      $form['revision_information']['#form_builder'] = array(
        'element_id' => 'revision_information',
        'element_type' => 'revision_information',
      );
    }
    else {
      unset($form['revision_information']);
    }

    $options_settings = node_get_form_settings($node_type, 'options');
    $form['options'] = node_options_fieldset($form['#node'], $options_settings);
    $form['options']['#options_settings'] = $options_settings;
    $form['options']['#form_builder'] = array(
      'element_id' => 'options',
      'element_type' => 'options',
    );

    $author_settings = node_get_form_settings($node_type, 'author');
    $form['author'] = node_author_fieldset($form['#node'], $author_settings);
    $form['author']['#author_settings'] = $author_settings;
    $form['author']['#form_builder'] = array(
      'element_id' => 'author',
      'element_type' => 'author',
    );

    unset($form['buttons']);

    return $form;
  }
}

/**
 * Implementation of hook_form_builder_save().
 */
function node_form_builder_save(&$form, $form_type, $form_id) {
  if ($form_type == 'node') {
    $node_type = $form_id;

    $node_element_ids = array(
      'title' => 'title',
      'body' => 'body_field',
      'revision' => 'revision_information',
      'options' => 'options',
      'author' => 'author',
    );
    $node_elements = form_builder_get_elements($form, $node_element_ids);

    foreach ($node_element_ids as $key => $element_id) {
      $settings = node_get_form_settings($node_type, $element_id);
      if (!isset($node_elements[$element_id])) {
        $settings['enabled'] = FALSE;
      }
      else {
        // Merge most settings from the #settings property.
        foreach ($node_elements[$element_id]['#' . $key . '_settings'] as $setting_key => $setting) {
          $settings[$setting_key] = $setting;
        }
        // If the field exists, it is enabled.
        $settings['enabled'] = TRUE;
        // Set the weight from the #weight property.
        $settings['weight'] = $node_elements[$element_id]['#weight'];
        // The title label is just the plain title attribute.
        if ($element_id == 'title') {
          $settings['label'] = $node_elements[$element_id]['#title'];
        }
        // Update settings if a fieldset.
        if (isset($node_elements[$element_id]['#type']) && $node_elements[$element_id]['#type'] == 'fieldset') {
          $settings['collapsible'] = !empty($node_elements[$element_id]['#collapsible']);
          $settings['collapsed'] = !empty($node_elements[$element_id]['#collapsed']);
        }
      }

      // Update the settings for this field.
      node_set_form_settings($node_type, $element_id, 'node', $settings);
    }

    $generic_field_ids = $form['#node_generic_fields'];
    $generic_fields = form_builder_get_elements($form, $generic_field_ids);
    foreach ($generic_fields as $key => $field) {
      if ($field['#form_builder']['element_type'] == 'generic_field') {
        $settings = array(
          'weight' => $field['#weight'],
        );
        node_set_form_settings($node_type, $key, 'node', $settings);
      }
    }
  }
}

/**
 * Implementation of hook_form_alter().
 *
 * @todo Merge this into node_form().
 */
function node_node_form_alter(&$form, &$form_state, $form_id) {
  // Set properties of the node form based on the node form settings.
  if (preg_match('/_node_form$/', $form_id)) {
    $type = $form['#node']->type;
    $settings = node_get_form_settings($type);

    $fields = array(
      'title',
      'body_field',
      'revision_information',
      'author',
      'options',
    );

    foreach ($fields as $key) {
      if (isset($settings[$key]['enabled']) && !$settings[$key]['enabled']) {
        $form[$key]['#access'] = FALSE;
      }
      else {
        switch ($key) {
          case 'title':
            $form[$key] = node_title_field($form['#node'], $settings[$key]);
            break;
          case 'revision_information':
            $form['revision_information'] = node_revision_fieldset($form['#node'], $settings[$key]);
            break;
          case 'author':
            $form[$key] = node_author_fieldset($form['#node'], $settings[$key]);
            break;
          case 'options':
            $form[$key] = node_options_fieldset($form['#node'], $settings[$key]);
            break;
        }
      }
    }

    // Set generic weights for any field controlled by node module.
    foreach ($settings as $key => $field_settings) {
      if (isset($form[$key]) && $field_settings['module'] == 'node') {
        $form[$key]['#weight'] = $field_settings['weight'];
      }
    }
  }
}

/**
 * A copy/paste out of node_content_form(). The title field.
 */
function node_title_field($node, $title_settings) {
  return array(
    '#type' => 'textfield',
    '#title' => check_plain($title_settings['label']),
    '#required' => TRUE,
    '#default_value' => $node->title,
    '#maxlength' => 255,
    '#weight' => $title_settings['weight'],
  );
}

/**
 * A straight copy/paste of a segment of node_form(). The node revision form.
 *
 * @param $node
 *   The node object being edited.
 * @param $revision_settings
 *   Settings specific to the revision fieldset.
 *
 * @todo Put this function directly in node.pages.inc, abstracting it from
 * node_form().
 */
function node_revision_fieldset($node, $revision_settings) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Revision information'),
    '#collapsible' => TRUE,
    // Collapsed by default when "Create new revision" is unchecked
    '#collapsed' => $revision_settings['collapsed'],
    '#weight' => $revision_settings['weight'],
  );
  $form['revision'] = array(
    '#access' => user_access('administer nodes'),
    '#type' => 'checkbox',
    '#title' => t('Create new revision'),
    '#default_value' => $revision_settings['revision'],
  );
  $form['log'] = array(
    '#type' => 'textarea',
    '#title' => t('Log message'),
    '#rows' => 2,
    '#description' => t('An explanation of the additions or updates being made to help other authors understand your motivations.'),
  );

  return $form;
}

/**
 * A straight copy/paste of a segment of node_form(). The authoring information form.
 *
 * @param $node
 *   The node object being edited.
 * @param $authoring_settings
 *   Settings specific to the authoring information fieldset.
 *
 * @todo Put this function directly in node.pages.inc, abstracting it from
 * node_form().
 */
function node_author_fieldset($node, $authoring_settings) {
  $form = array(
    '#type' => 'fieldset',
    '#access' => user_access('administer nodes'),
    '#title' => t('Authoring information'),
    '#collapsible' => $authoring_settings['collapsible'],
    '#collapsed' => $authoring_settings['collapsed'],
    '#weight' => $authoring_settings['weight'],
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Authored by'),
    '#maxlength' => 60,
    '#autocomplete_path' => 'user/autocomplete',
    '#default_value' => isset($node->name) ? $node->name : '',
    '#weight' => -1,
    '#description' => t('Leave blank for %anonymous.', array('%anonymous' => variable_get('anonymous', t('Anonymous')))),
  );
  $form['date'] = array(
    '#type' => 'textfield',
    '#title' => t('Authored on'),
    '#maxlength' => 25,
    '#description' => t('Format: %time. Leave blank to use the time of form submission.', array('%time' => !empty($node->date) ? $node->date : format_date($node->created, 'custom', 'Y-m-d H:i:s O'))),
  );

  if (isset($node->date)) {
    $form['date']['#default_value'] = $node->date;
  }

  return $form;
}

/**
 * A straight copy/paste of a segment of node_form(). The publishing options form.
 *
 * @param $node
 *   The node object being edited.
 * @param $publishing_settings
 *   Settings specific to the publishing options fieldset.
 *
 * @todo Put this function directly in node.pages.inc, abstracting it from
 * node_form().
 */
function node_options_fieldset($node, $publishing_settings) {
  $form = array(
    '#type' => 'fieldset',
    '#access' => user_access('administer nodes'),
    '#title' => t('Publishing options'),
    '#collapsible' => $publishing_settings['collapsible'],
    '#collapsed' => $publishing_settings['collapsed'],
    '#weight' => $publishing_settings['weight'],
  );
  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Published'),
    '#default_value' => $node->status,
  );
  $form['promote'] = array(
    '#type' => 'checkbox',
    '#title' => t('Promoted to front page'),
    '#default_value' => $node->promote,
  );
  $form['sticky'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sticky at top of lists'),
    '#default_value' => $node->sticky,
  );

  return $form;
}

/**
 * Retrieve node form settings for a particular field.
 */
function node_get_form_settings($node_type, $field = NULL, $reset = FALSE) {
  static $form_settings = array();

  if ($reset) {
    $form_settings = array();
    return;
  }

  if (!isset($form_settings[$node_type])) {
    // Get settings from the node form settings table.
    $form_settings[$node_type] = array();
    $result = db_select('node_form_settings', 'nfs', array('fetch' => PDO::FETCH_ASSOC))
      ->fields('nfs')
      ->condition('nfs.type', $node_type)
      ->execute();
    foreach ($result as $settings) {
      $form_settings[$node_type][$settings->field] = unserialize($settings->settings);
      $form_settings[$node_type][$settings->field]['module'] = $settings->module;
    }

    // Get settings from the node type table.
    $type = node_type_get_type($node_type);
    $form_settings[$node_type]['title']['enabled'] = $type->has_title ? TRUE : FALSE;
    $form_settings[$node_type]['title']['label'] = isset($type->title_label) ? $type->title_label : '';
    $form_settings[$node_type]['body_field']['enabled'] = $type->has_body ? TRUE : FALSE;
    $form_settings[$node_type]['body_field']['label'] = isset($type->body_label) ? $type->body_label : '';

    // Merge default settings from modules.
    foreach (module_implements('node_form_default_settings') as $module) {
      $additional_settings = module_invoke($module, 'node_form_default_settings', $node_type);
      if (!empty($additional_settings)) {
        foreach ($additional_settings as $field_id => $field_settings) {
          $form_settings[$node_type][$field_id]['module'] = $module;
          if (isset($form_settings[$node_type][$field_id])) {
            $form_settings[$node_type][$field_id] = array_merge($field_settings, $form_settings[$node_type][$field_id]);
          }
          else {
            $form_settings[$node_type][$field_id] = $field_settings;
          }
        }
      }
    }
  }

  if (!isset($field)) {
    return $form_settings[$node_type];
  }
  elseif (isset($form_settings[$node_type][$field])) {
    return $form_settings[$node_type][$field];
  }

  return FALSE;
}

/**
 * Set the node form settings for a particular field.
 */
function node_set_form_settings($node_type, $field, $module, $settings) {
  $serialized_value = serialize($settings);
  db_merge('node_form_settings')
    ->key(array('type' => $node_type, 'field' => $field))
    ->fields(array(
      'settings' => $serialized_value,
      'module' => $module
    ))
    ->execute();

  // Special cases for node type settings.
  if ($field == 'body_field') {
    $type = node_type_get_type($node_type);
    $type->has_body = $settings['enabled'];
    $type->body_label = $settings['label'];
    $type->min_word_count = $settings['min_word_count'];
    node_type_save($type);
    node_types_rebuild();
    menu_rebuild();
  }
  if ($field == 'title') {
    $type = node_type_get_type($node_type);
    $type->has_title = $settings['enabled'];
    $type->title_label = $settings['label'];
    node_type_save($type);
    node_types_rebuild();
    menu_rebuild();
  }

  // Reset the static cache for the node form settings.
  node_get_form_settings(NULL, NULL, TRUE);
}

/**
 * Delete the node form settings for a node type, field, or module.
 *
 * All parameters all optional, but at least one must be specified to have any
 * effect. Multiple parameters may be passed in to further narrow the settings
 * that will be deleted.
 *
 * @param $node_type
 *   Optional. String of the node type whose settings are to be deleted.
 * @param $field
 *   Optional. The key of the field whose setting will be deleted.
 * @param $module
 *   Options. Name of the module that provided the field settings to be deleted.
 */
function node_delete_form_settings($node_type = NULL, $field = NULL, $module = NULL) {
  $delete = db_delete('node_form_settings');
  if (isset($node_type)) {
    $delete->condition('type', $node_type);
  }
  if (isset($field)) {
    $delete->condition('field', $field);
  }
  if (isset($field)) {
    // @todo: bogus conditional?
    $delete->condition('module', $module);
  }

  if (isset($node_type) || isset($field)) {
    // @todo: bogus conditional?
    $delete->execute();

    // Reset the static cache for the node form settings.
    node_get_form_settings(NULL, NULL, TRUE);
  }
}

/**
 * Implementation of hook_node_form_default_settings().
 */
function node_node_form_default_settings() {
  $defaults = array();

  $defaults['title'] = array(
    'enabled' => TRUE,
    'label' => t('Title'),
    'weight' => -5,
  );
  $defaults['body_field'] = array(
    'enabled' => TRUE,
    'label' => t('Body'),
    'min_word_count' => 0,
    'weight' => 0,
  );
  $defaults['revision_information'] = array(
    'enabled' => TRUE,
    'collapsible' => TRUE,
    'collapsed' => TRUE,
    'weight' => 20,
    'revision' => FALSE,
  );
  $defaults['author'] = array(
    'collapsible' => TRUE,
    'collapsed' => TRUE,
    'weight' => 21,
  );
  $defaults['options'] = array(
    'collapsible' => TRUE,
    'collapsed' => TRUE,
    'status' => 1,
    'promote' => 0,
    'sticky' => 0,
    'weight' => 25,
  );

  return $defaults;
}
