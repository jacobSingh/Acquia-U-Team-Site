<?php

/**
 * @file
 * Admin settings page callbacks for the font_management module.
 */

/**
 * Menu callback; displays the font_management module settings page.
 *
 * @see system_settings_form()
 */
function font_management_admin_settings($form, &$form_state) {
  // Fonts.com section.

  $saved_token = variable_get('font_management_webfonts_token', '');
  $saved_project_key = variable_get('font_management_webfonts_project', '');

  $form['webfonts'] = array(
    '#markup' => '<h2>' . t('Fonts.com Web Fonts') . '</h2>',
  );

  $form['font_management_webfonts_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Fonts.com web fonts'),
    '#default_value' => variable_get('font_management_webfonts_enable', FALSE),
  );
  $form['font_management_webfonts_token'] = array(
    '#type' => 'textfield',
    '#default_value' => $saved_token,
    '#size' => 130,
    '#title' => t('Fonts.com Web Fonts authentication key'),
    '#description' => t('Required to use Fonts.com Web Fonts with this site. To get your Fonts.com Web Fonts authentication key, create an account on <a href="@link">fonts.com</a>, then visit My Account and click the Account summary tab.', array('@link' => 'http://webfonts.fonts.com')),
    '#ajax' => array(
      'callback' => 'font_management_webfonts_token_callback',
      'wrapper' => 'webfonts-project-list-replace',
    ),
  );

  $entered_token = isset($form_state['values']['font_management_webfonts_token']) ? $form_state['values']['font_management_webfonts_token'] : '';
  $options = array('' => t('None selected'));
  $default_project = '';

  // If we have a token, try getting a project list from Monotype.
  if (!empty($saved_token) || !empty($entered_token)) {
    $token = (empty($entered_token) ? $saved_token : $entered_token);
    try {
      $projects = FontManagementMonotype::getInstance()->getProjectList($token);
      foreach ($projects->Project as $index => $project) {
        $options[(string)$project->ProjectKey] = (string)$project->ProjectName;
        if ($saved_project_key == (string)$project->ProjectKey) {
          $default_project = $saved_project_key;
        }
      }
      if (count($options) > 1) {
        unset($options['']);
      }
    }
    catch (FontManagementMonotypeException $e) {
      // Only display errors on the form itself (not while processing an AJAX
      // callback, since we don't want the form to show an error while the user
      // is still typing in their token).
      if (empty($form_state['input'])) {
        font_management_monotype_handle_exception($e);
      }
    }
  }
  // Display a link to Fonts.com for people who haven't entered a token.
  else {
    $form['font_management_webfonts_enable']['#prefix'] = '<p><strong>' . t('With a <a href="@fonts_com_link">Fonts.com Web Fonts</a> account you can choose from thousands of classic fonts. <a href="@fonts_com_link">Try it for free.</a>', array('@fonts_com_link' => 'http://webfonts.fonts.com')) . '</strong></p>';
  }

  $form['font_management_webfonts_project'] = array(
    '#type' => 'select',
    '#title' => t('Fonts.com Web Fonts project'),
    '#options' => $options,
    '#default_value' => $default_project,
    '#prefix' => '<div id="webfonts-project-list-replace">',
    '#suffix' => '</div>',
    );

  // Typekit section.

  $form['typekit'] = array(
    '#markup' => '<h2>' . t('Typekit') . '</h2>',
    '#prefix' => '<hr>',
  );

  $form['font_management_typekit_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Typekit fonts'),
    '#default_value' => variable_get('font_management_typekit_enable', FALSE),
  );

  $form['font_management_typekit_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Typekit ID'),
    '#size' => 10,
    '#description' => t('Required to use Typekit fonts with this site. To get your Typekit ID, create an account on <a href="@link">typekit.com</a>, add your font(s), select "Launch Kit Editor," select "Embed Code," and the ID will be found at the bottom where it says "If you\'re using a plugin or service that asks for a Typekit Kit ID, use this: xxxxxxx"', array('@link' => 'http://typekit.com')),
    '#default_value' => variable_get('font_management_typekit_key', ''),
  );

  // Display a link to Typekit for people who haven't entered a key.
  if (empty($form['font_management_typekit_key']['#default_value'])) {
    $form['font_management_typekit_enable']['#prefix'] = '<p><strong>' . t("With a <a href='@link'>Typekit</a> account you can choose from hundreds of great fonts. <a href='@pricing'>Try it for free.</a>", array('@link' => 'http://typekit.com', '@pricing' => 'https://typekit.com/plans')) . '</strong></p>';
  }

  // Make a system settings form and return.
  return system_settings_form($form);
}

/**
 * Ajax callback for font_management_admin_settings().
 */
function font_management_webfonts_token_callback($form, $form_state) {
  return $form['font_management_webfonts_project'];
}
