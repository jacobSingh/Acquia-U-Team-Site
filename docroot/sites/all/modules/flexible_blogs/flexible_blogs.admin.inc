<?php

/**
 * @file
 * Administrative page callbacks for the Flexible blogs module.
 */

/**
 * Form callback: builds the form for editing blog settings.
 *
 * @param $form
 *   An associative array containing the structure of the form.
 * @param $form_state
 *   An associative array containing the current state of the form.
 *
 * @return
 *   An array representing the form definition.
 *
 * @ingroup forms
 */
function flexible_blogs_admin_settings($form, &$form_state) {
  $form['flexible_blogs_node_link_type'] = array(
    '#type' => 'radios',
    '#title' => t('Blog entry link'),
    '#description' => t('Choose the type of link that will be added to each blog entry.'),
    '#options' => array(
      FLEXIBLE_BLOGS_NODE_LINK_NONE => t('No link'),
      FLEXIBLE_BLOGS_NODE_LINK_AUTHOR => t("Link to author's blog"),
      FLEXIBLE_BLOGS_NODE_LINK_SITE => t('Link to site-wide blog'),
    ),
    '#default_value' => variable_get('flexible_blogs_node_link_type', FLEXIBLE_BLOGS_NODE_LINK_SITE),
  );

  return system_settings_form($form);
}

