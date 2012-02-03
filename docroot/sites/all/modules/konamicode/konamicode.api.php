<?php
/**
 * @file
 * API for the Konami Code Drupal module
 */

/**
 * Describes all the Konami Code actions available.
 *
 * @return
 *   An associative array where the key is the machine name of the Konami Code
 *   action, and the value is the human readable name of the action.
 */
function hook_konamicode() {
  return array(
    'redirect' => t('Redirect'),
  );
}

/**
 * Invoked when the selected Konami Code is used.
 *
 * Replace ACTION with the name of the Konami Code action.
 */
function hook_konamicode_ACTION() {
  drupal_add_js(drupal_get_path('module', 'konamicode') . '/konamicode-redirect.js');
}

/**
 * Provides all the actions for the given Konami Code.
 *
 * @return
 *   A form array. Note that you must use element names that are unique the
 *   given action as they will be used in a system settings form. This means
 *   that all the elements will end up as variables with the same names.
 */
function hook_konamicode_ACTION_settings() {
  $form['konamicode_redirect_destination'] = array(
    // ...
    '#default_value' => variable_get('konamicode_redirect_destination', ''),
  );
  return $form;
}


/**
 * JavaScript code in konamicode-redirect.js is as follows:
 * Replace ACTION with the name of your action.
 *
 * Drupal.konamicode_ACTION = function() {
 *   window.location = 'http://bacolicio.us/' + window.location;
 * };
 *
 */
