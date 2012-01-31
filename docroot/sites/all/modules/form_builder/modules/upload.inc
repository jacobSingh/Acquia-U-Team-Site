<?php

/**
 * @file
 *   Form builder implementation for upload.module.
 */

/**
 * Implementation of hook_form_builder_types().
 */
function upload_form_builder_types() {
  $fields = array();

  // Make a pseudo node for the upload form.
  $node = (object) array(
    'type' => '',
    'files' => array(),
  );

  // Make a default path settings to pass into the upload form.
  $settings = module_invoke('upload', 'node_form_default_settings');

  $fields['upload_settings'] = array(
    'title' => t('Attachments'),
    'properties' => array(
      'collapsible',
      'collapsed',
    ),
    'default' => upload_fieldset($node, $settings['attachments']),
    'unique' => TRUE,
    'removable' => TRUE,
    'configurable' => TRUE,
    'palette_group' => 'special',
  );

  return array(
    'node' => $fields,
    'example' => $fields,
  );
}

/**
 * Implementation of hook_form_builder_load_alter().
 */
function upload_form_builder_load_alter(&$form, $form_type, $form_id) {
  if ($form_type == 'node' && isset($form['attachments'])) {
    $form['attachments']['#form_builder'] = array(
      'element_id' => 'upload_settings',
      'element_type' => 'upload_settings',
    );
    // Unset the the attach button.
    unset($form['attachments']['wrapper']['new']['attach']['#ahah']);
    $form['attachments']['wrapper']['new']['attach']['#disabled'] = TRUE;
  }
}

/**
 * Implementation of hook_form_builder_save().
 */
function upload_form_builder_save(&$form, $form_type, $form_id) {
  if ($form_type == 'node') {
    $element = form_builder_get_element($form, 'upload_settings');
    $node_type = $form_id;

    if ($element) {
      $settings = array(
        'enabled' => TRUE,
        'collapsible' => $element['#collapsible'],
        'collapsed' => $element['#collapsed'],
        'weight' => $element['#weight'],
      );
      variable_set('upload_' . $node_type, '1');
    }
    else {
      $settings = array(
        'enabled' => FALSE,
      );
      variable_set('upload_' . $node_type, '0');
    }

    node_set_form_settings($form_id, 'attachments', 'upload', $settings);
  }
}

/**
 * Implementation of hook_form_alter().
 *
 * @todo Merge this into upload_form_alter().
 */
function upload_node_form_alter(&$form, &$form_state, $form_id) {
  // Set properties of the path form item based on the path node form settings.
  if (isset($form['#node']) && $form['#node']->type .'_node_form' == $form_id) {
    if (isset($form['attachments'])) {
      $settings = node_get_form_settings($form['#node']->type, 'attachments');
      $form['attachments']['#weight'] = $settings['weight'];
      $form['attachments']['#collapsible'] = $settings['collapsible'];
      $form['attachments']['#collapsed'] = $settings['collapsed'];
    }
  }
}

/**
 * Implementation of hook_node_form_default_settings().
 *
 * Get the settings for the path fieldset on the node form.
 */
function upload_node_form_default_settings($type = NULL) {
  $defaults = array();
  $defaults['attachments'] = array(
    'enabled' => TRUE,
    'collapsible' => TRUE,
    'collapsed' => TRUE,
    'weight' => 30,
  );
  return $defaults;
}


/**
 * A straight copy/paste of path_form_alter(). Get the path node form.
 *
 * @param $path
 *   The contents of $node->path.
 * @param $path_settings
 *   An array with configuring properties of the path fieldset.
 *
 * @todo Put this function directly in path.module, abstracting it from
 * path_form_alter().
 */
function upload_fieldset($node, $upload_settings) {
  // Attachments fieldset
  $form = array(
    '#type' => 'fieldset',
    '#access' => user_access('upload files'),
    '#title' => t('File attachments'),
    '#collapsible' => $upload_settings['collapsible'],
    '#collapsed' => empty($node->files) && $upload_settings['collapsed'],
    '#description' => t('Changes made to the attachments are not permanent until you save this post. The first "listed" file will be included in RSS feeds.'),
    '#prefix' => '<div class="attachments">',
    '#suffix' => '</div>',
    '#weight' => 30,
  );

  // Wrapper for fieldset contents (used by ahah.js).
  $form['wrapper'] = array(
    '#prefix' => '<div id="attach-wrapper">',
    '#suffix' => '</div>',
  );

  // Make sure necessary directories for upload.module exist and are
  // writable before displaying the attachment form.
  $path = file_directory_path();
  $temp = file_directory_temp();
  // Note: pass by reference
  if (!file_check_directory($path, FILE_CREATE_DIRECTORY) || !file_check_directory($temp, FILE_CREATE_DIRECTORY)) {
    $form['#description'] =  t('File attachments are disabled. The file directories have not been properly configured.');
    if (user_access('administer site configuration')) {
      $form['#description'] .= ' '. t('Please visit the <a href="@admin-file-system">file system configuration page</a>.', array('@admin-file-system' => url('admin/settings/file-system')));
    }
    else {
      $form['#description'] .= ' '. t('Please contact the site administrator.');
    }
  }
  elseif (drupal_function_exists('_upload_form')) {
    $form['wrapper'] += _upload_form($node);
  }

  return $form;
}
