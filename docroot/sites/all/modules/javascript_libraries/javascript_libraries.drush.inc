<?php

/**
 * @file
 *   Drush commands for managing JavaScript libraries.
 */

/**
 * Implements hook_drush_command().
 */
function javascript_libraries_drush_command() {
  $items['jslib-switch-url'] = array(
    'description' => 'Switches the URL of an external JavaScript library.',
    'arguments' => array(
      'current_url' => 'The current URL of the library.',
      'new_url' => 'The new URL of the library.',
    ),
    'examples' => array(
      'drush jslib-switch-url http://www.example.com/js/somelibrary-v1.js http://www.example.com/js/somelibrary-v2.js' => 'Switches the site to use a new version of an external JavaScript library hosted at example.com.',
    ),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function javascript_libraries_drush_help() {
  switch ($section) {
    case 'drush:jslib-switch-url':
      return dt('Usage: drush jslib-switch-url <current_url> <new_url>.');
  }
}

/**
 * Drush command callback; switches the URL of an external JavaScript library.
 */
function drush_javascript_libraries_jslib_switch_url($current_url = NULL, $new_url = NULL) {
  // The new URL must be valid in order to proceed.
  if (!javascript_libraries_valid_external_url($new_url)) {
    return drush_set_error(NULL, dt('The URL @url is invalid; it must begin with http:// or https:// and end with .js or .js.txt.', array('@url' => $new_url)));
  }

  // Replace all occurrences of the old URL with the new one.
  $changed = FALSE;
  $libraries = variable_get('javascript_libraries_custom_libraries', array());
  foreach ($libraries as &$library) {
    if ($library['type'] == 'external' && $library['uri'] == $current_url && $library['uri'] != $new_url) {
      $library['uri'] = $new_url;
      $changed = TRUE;
    }
  }

  // Only save the variable if it changed.
  if ($changed) {
    drush_op('variable_set', 'javascript_libraries_custom_libraries', $libraries);
    drush_log(dt('External library URL was updated from @old to @new.', array('@old' => $current_url, '@new' => $new_url)), 'success');
  }
  else {
    drush_log(dt('There are no existing libraries with the URL @old.', array('@old' => $current_url)), 'warning');
  }
}
