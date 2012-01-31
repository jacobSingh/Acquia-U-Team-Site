<?php

/**
 * @file
 * Functions related to plugin and module overrides.
 */

/**
 * Implements hook_wysiwyg_editor_settings_alter().
 */
function gardens_features_wysiwyg_editor_settings_alter(&$settings, &$context) {
  // CKEditor 3.5 added an IFRAME plugin and a flash plugin. They convert
  // width="100%" to width="100". Until that's fixed, don't use these plugins.
  // The flash plugin is overriden in the gardens_features module.
  if ($context['editor']['name'] == 'ckeditor' && version_compare($context['editor']['installed version'], '3.5.0.6260', '>=')) {
    if (empty($settings['removePlugins'])) {
      $settings['removePlugins'] = 'iframe';
    }
    elseif (strpos($settings['removePlugins'], 'iframe') === FALSE) {
      $settings['removePlugins'] .= ',iframe';
    }
  }
  // Remove the gardens-flash and gardens-wmode plugin buttons. The functionality
  // remains, but we don't want buttons in the WYSIWYG UI.
  $custom_buttons = array('gardens-flash', 'gardens-wmode');
  if ($context['editor']['name'] == 'ckeditor' && !empty($settings['toolbar'])) {
    foreach ($settings['toolbar'] as $i => $toolbar ) {
      foreach ($toolbar as $j => $button) {
        if (in_array($button, $custom_buttons)) {
          unset($settings['toolbar'][$i][$j]);
        }
      }
    }
  }
}

/**
 * Implements hook_wysiwyg_include_directory().
 */
function gardens_features_wysiwyg_include_directory($type) {
  switch ($type) {
    case 'plugins':
      return 'wysiwyg_plugins';
        break;
  }
}
