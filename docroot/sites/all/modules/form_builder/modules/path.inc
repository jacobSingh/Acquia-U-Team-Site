<?php

/**
 * @file
 *   Form builder implementation for path.module.
 */

/**
 * Implementation of hook_form_builder_types().
 */
function path_form_builder_types() {
  $fields = array();

  // Make a default path settings to pass into the path form.
  $settings = module_invoke('path', 'node_form_default_settings');

  $fields['path_settings'] = array(
    'title' => t('Path'),
    'properties' => array(
      'collapsible',
      'collapsed',
    ),
    'default' => path_fieldset('', $settings['path']),
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
function path_form_builder_load_alter(&$form, $form_type, $form_id) {
  if ($form_type == 'node') {
    $settings = node_get_form_settings($form_id, 'path');
    if ($settings['enabled']) {
      $form['path'] = path_fieldset('', $settings);
      $form['path']['#form_builder'] = array(
        'element_id' => 'path_settings',
        'element_type' => 'path_settings',
      );
    }
  }
}

/**
 * Implementation of hook_form_builder_save().
 */
function path_form_builder_save(&$form, $form_type, $form_id) {
  if ($form_type == 'node') {
    $element = form_builder_get_element($form, 'path_settings');
    $node_type = $form_id;

    if ($element) {
      $settings = array(
        'enabled' => TRUE,
        'collapsible' => $element['#collapsible'],
        'collapsed' => $element['#collapsed'],
        'weight' => $element['#weight'],
      );
    }
    else {
      $settings = array(
        'enabled' => FALSE,
      );
    }

    node_set_form_settings($form_id, 'path', 'path', $settings);
  }
}

/**
 * Implementation of hook_form_alter().
 *
 * @todo Merge this into path_form_alter().
 */
function path_node_form_alter(&$form, &$form_state, $form_id) {
  // Set properties of the path form item based on the path node form settings.
  if (isset($form['#node']) && $form['#node']->type .'_node_form' == $form_id) {
    $settings = node_get_form_settings($form['#node']->type, 'path');

    if ($settings['enabled']) {
      $path = isset($form['#node']->path) ? $form['#node']->path : '';
      $form['path'] = path_fieldset($path, $settings);
    }
    else {
      $form['path']['#access'] = FALSE;
    }
  }
}

/**
 * Implementation of hook_node_form_default_settings().
 *
 * Get the settings for the path fieldset on the node form.
 */
function path_node_form_default_settings($type = NULL) {
  $defaults = array();
  $defaults['path'] = array(
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
function path_fieldset($path, $path_settings) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('URL path settings'),
    '#collapsible' => $path_settings['collapsible'],
    '#collapsed' => $path_settings['collapsed'],
    '#access' => user_access('create url aliases'),
    '#weight' => $path_settings['weight'],
  );
  $form['path'] = array(
    '#type' => 'textfield',
    '#default_value' => $path,
    '#maxlength' => 250,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Optionally specify an alternative URL by which this node can be accessed. For example, type "about" when writing an about page. Use a relative path and don\'t add a trailing slash or the URL alias won\'t work.'),
  );
  if ($path) {
    $form['pid'] = array(
      '#type' => 'value',
      '#value' => db_query("SELECT pid FROM {url_alias} WHERE dst = :dst AND language = :language", array('path' => $path, 'language' => $form['#node']->language))->fetchField()
    );
  }

  return $form;
}
