<?php

/**
 * @file
 * Creates a CKEditor plugin through the wysiwyg module's plugin API.
 */

/**
 * Implements WYSIWYG's hook_INCLUDE_plugin().
 *
 * If wmode is not set to opaque as a parameter to the object tag or if
 * wmode is not set to opaque as an attribute of the embed tag, the flash object
 * will appear to hover above all other elements on the page.
 *
 * This plugin ensures that wmode is set to transparent for embed and object tags.
 */
function gardens_features_gardens_wmode_plugin() {
  // Plugin definition
  $plugins['gardens-wmode'] = array(
    'title' => t('Force wmode transparent'),
    'vendor url' => 'http://www.acquia.com',
    'icon path' => NULL,
    'icon file' => NULL,
    'icon title' => NULL,
    'js path' => drupal_get_path('module', 'gardens_features') . '/wysiwyg_plugins/gardens-wmode',
    'js file' => 'gardens-wmode.js',
    'css file' => NULL,
    'css path' => NULL,
    'settings' => array(),
  );

  return $plugins;
}