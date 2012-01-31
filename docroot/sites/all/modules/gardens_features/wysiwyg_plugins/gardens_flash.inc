<?php

/**
 * @file
 * Creates a CKEditor plugin through the wysiwyg module's plugin API.
 */

/**
 * Implements WYSIWYG's hook_INCLUDE_plugin().
 */
function gardens_features_gardens_flash_plugin() {
  // Plugin definition
  $plugins['gardens-flash'] = array(
    'title' => t('Allow 100% flash width'),
    'vendor url' => 'http://www.acquia.com',
    'icon path' => NULL,
    'icon file' => NULL,
    'icon title' => NULL,
    'js path' => drupal_get_path('module', 'gardens_features') . '/wysiwyg_plugins/gardens-flash',
    'js file' => 'gardens-flash.js',
    'css file' => NULL,
    'css path' => NULL,
    'settings' => array(),
  );

  return $plugins;
}
