<?php
// $Id: typekit.admin.inc,v 1.6 2010/05/17 14:42:45 jpamental Exp $

/**
 * @file
 * This file holds the functions for the Typekit admin settings.
 *
 * @ingroup typekit
 */

/**
 * Menu callback; displays the Typekit module settings page.
 *
 * @see system_settings_form()
 */
function typekit_admin_settings() {
  $form = array();
  $php_access = module_exists('php') && user_access('use PHP for settings');
  $code = htmlspecialchars('<script type="text/javascript" src="http://use.typekit.com/XXXXXXX.js">');
  $key_description = t('
    This is the key for your site, and Typekit will not work without it.  This can be found in the embed code that you get for each kit.  It is the name of the JS include file.
    <br /><code>
      !code
    </code>
  ', array('!code' => $code));
  $visibility_desc = t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));
  $visibility_options = array(
    0 => t('All pages except those listed'),
    1 => t('Only the listed pages'),
  );

  // Check if user has access to do PHP stuff.
  if ($php_access) {
    $visibility_options[3] = t('Pages on which this PHP code returns <code>TRUE</code> (experts only)');
    $visibility_desc .= ' ' . t('If the PHP option is chosen, enter PHP code between %php. Note that executing incorrect PHP code can break your Drupal site.', array('%php' => '<?php ?>'));
  }

  // Enable
  $form['typekit_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Enables Typekit on your site.  On by default.'),
    '#default_value' => variable_get('typekit_enable', TRUE),
  );

  // Key
  $form['typekit_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Typekit Key'),
    '#description' => $key_description,
    '#default_value' => variable_get('typekit_key', ''),
  );

  // Visibility settings
  $form['typekit_visibility'] = array(
    '#type' => 'fieldset',
    '#title' => t('Visibility'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['typekit_visibility']['typekit_visibility_settings'] = array(
    '#type' => 'radios',
    '#title' => t('Custom visibility settings'),
    '#options' => $visibility_options,
    '#description' => t('Allows Typekit to show only on certain pages.'),
    '#default_value' => variable_get('typekit_visibility_settings', 0),
  );
  $form['typekit_visibility']['typekit_visibility_pages'] = array(
    '#type' => 'textarea',
    '#title' => t('Pages'),
    '#default_value' => variable_get('typekit_visibility_pages', ''),
    '#description' => $visibility_desc,
  );

  // Make a system settings form and return.
  return system_settings_form($form);
}
