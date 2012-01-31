<?php
/**
 * Menu callback; server settings form.
 */
function server_variables_admin_settings($form, &$form_state) {
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'server_variables') . '/server_variables.js',
  );
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'server_variables') . '/server_variables.admin.css',
  );
  $form['help']['#markup'] = t('<div class="form-item">Enable fields to expose in JavaScript. Variables are added to the Drupal.settings.server_variables object. !help</div>',
          array('!help' => l('Help', 'admin/help/server_variables')));
  // site-wide fields
  $form['site'] = array();
  server_variables_add_form_elements($form, 'site', 'site_name', t('Site name'), 'site_name');
  server_variables_add_form_elements($form, 'site', 'page_title', t('Page title'), 'page_title');
  // authenticated user
  $form['user'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fields that track authenticated users'),
    '#collapsible' => TRUE,
  );
  server_variables_add_form_elements($form, 'user', 'mail', t('Email'), 'user_mail');
  server_variables_add_form_elements($form, 'user', 'uid', t('User ID'), 'user_id');
  server_variables_add_form_elements($form, 'user', 'name', t('Username'), 'user_name');
  server_variables_add_form_elements($form, 'user', 'roles', t('Roles'), 'user_roles');
  // user custom field
  foreach (field_info_instances('user', 'user') as $field) {
    $field_name = $field['field_name'];
    $field_info = field_info_field($field_name);
    if ( array_key_exists('value', $field_info['columns']) ) {
      $title = $field['label'];
      server_variables_add_form_elements($form, 'user', $field_name, $title, "user_$field_name");
    }
  }
  // action settings
  $form['action'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fields that track actions on entities'),
    '#collapsible' => TRUE,
  );
  server_variables_add_form_elements($form, 'action', 'op', t('Operation type'), 'action_op',
          t('Exposes the variable "insert" for a newly created item, "update" for a recently updated item, "delete" on item deletion or "view" for display.'));
  server_variables_add_form_elements($form, 'action', 'id', t('Entity ID'), 'entity_id',
          t('Exposes the ID of comments, content, media and users'));
  server_variables_add_form_elements($form, 'action', 'type', t('Entity type'), 'entity_type',
          t('Exposes "comment", "node" (for content), "media" and "user"'));
  server_variables_add_form_elements($form, 'action', 'subtype', t('Entity subtype'), 'entity_subtype',
          t('Exposes content type, e.g. "blog"'));
  server_variables_add_form_elements($form, 'action', 'title', t('Entity title'), 'entity_title',
          t('Exposes the title of comments, content and media'));
  server_variables_add_form_elements($form, 'action', 'create_date', t('Entity create date'), 'create_date', t('Exposes the creation date of comments, content, media and users'));
  server_variables_add_form_elements($form, 'action', 'update_date', t('Entity update date'), 'update_date', t('Exposes the update date of comments, content, media and users'));
  server_variables_add_form_elements($form, 'action', 'taxonomy', t('Taxonomy'), 'entity_taxonomy');

  $form['actuser'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fields that track user actions'),
    '#collapsible' => TRUE,
  );
  server_variables_add_form_elements($form, 'actuser', 'mail', t('Email'), 'actuser_mail');
  server_variables_add_form_elements($form, 'actuser', 'uid', t('User ID'), 'actuser_id');
  server_variables_add_form_elements($form, 'actuser', 'name', t('Username'), 'actuser_name');
  server_variables_add_form_elements($form, 'actuser', 'roles', t('Roles'), 'actuser_roles');
  // user custom field
  foreach (field_info_instances('user', 'user') as $field) {
    $field_name = $field['field_name'];
    $field_info = field_info_field($field_name);
    if ( array_key_exists('value', $field_info['columns']) ) {
      $title = $field['label'];
      server_variables_add_form_elements($form, 'actuser', $field_name, $title, "actuser_$field_name");
    }
  }
  $form = system_settings_form($form);
  return $form;
}


/**
 * Validation callback for server_variables_admin_settings form.
 */
function server_variables_admin_settings_validate($form, &$form_state) {
  $values = $form_state['values'];
  $checked = FALSE;
  foreach ($values as $key => $value) {
    if (preg_match('/^server_variables_/', $key)) {
      if (substr($key, -3) == 'ind') {
        // field is a checkbox
        $checked = $value;
        $fieldset = preg_replace('/^server_variables_(\w+?)_.+?_ind$/', '$1', $key);
        $title = $form[$fieldset][$key]['#title'];
      }
      else {
        // field is a textbox - check value if corresponding checkbox was set
        if ($checked && empty($value)) {
          form_set_error($key, t('Variable name for @name is required.', array('@name' => $title)));
        }
      }
    }
  }
}


/**
 * Add a set of fields to form (checkbox and field) to select variables for inclusion in JavaScript.
 *
 * @param $form (byref) the form array
 * @param $group name of fieldset group
 * @param $field name of field
 * @param $title  title of field
 */
function server_variables_add_form_elements(&$form, $group, $field, $title, $default = NULL, $help = '') {
  $form[$group]["server_variables_{$group}_{$field}_ind"] = array(
    '#type' => 'checkbox',
    '#title' => $title,
    '#default_value' => variable_get("server_variables_{$group}_{$field}_ind", FALSE),
  );
  $form[$group]["server_variables_{$group}_{$field}_var"] = array(
    '#type' => 'textfield',
    '#title' => 'Variable',
    '#default_value' => variable_get("server_variables_{$group}_{$field}_var", $default),
    '#description' => $help,
  );
}
