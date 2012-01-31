<?php
/**
 * @file
 * API documentation for JavaScript Libraries Manager module.
 */

/**
 * @defgroup mollom_api Mollom API
 * @{
 * Functions to integrate with Mollom form protection.
 *
 */

/**
 * Implements hook_javascript_libraries_available_alter().
 *
 * This alter hook allows a module to add or remove JS libraries
 * that will be exposed to the end user, as well as changing the
 * grouping of them in the UI.
 */
function hook_javascript_libraries_available_alter(&$groups) {
  // Don't show jQuery UI Tabs as an option.
  $idx = array_search('ui.tabs', $groups['jQuery UI']['library']);
  if ($idx !== FALSE) {
    unset($groups['jQuery UI']['library'][$idx]);
  }
}