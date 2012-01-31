<?php

/**
 * @file
 *   Form builder implementation for poll.module.
 *
 *   Note this file doesn't actually do much of anything since node.inc
 *   now handles generic weight handling. But poll-specific functionality
 *   could be added here.
 */

/**
 * Implementation of hook_form_builder_types().
 */
function poll_form_builder_types() {
  $fields = array();

  $fields['poll_choices'] = array(
    'title' => t('Menu'),
    'properties' => array(),
    'default' => array(),
    'unique' => TRUE,
    'configurable' => FALSE,
    'removable' => FALSE,
    'palette_group' => 'special',
  );

  $fields['poll_settings'] = array(
    'title' => t('Menu'),
    'properties' => array(),
    'default' => array(),
    'unique' => TRUE,
    'configurable' => FALSE,
    'removable' => FALSE,
    'palette_group' => 'special',
  );

  return array(
    'node' => $fields,
  );
}

/**
 * Implementation of hook_form_builder_load_alter().
 */
function poll_form_builder_load_alter(&$form, $form_type, $form_id) {
  if ($form_type == 'node' && $form_id == 'poll') {

    $choice_settings = node_get_form_settings($form_id, 'poll_choices');
    $form['choice_wrapper']['#weight'] = $choice_settings['weight'];
    $form['choice_wrapper']['#form_builder'] = array(
      'element_id' => 'poll_choices',
      'element_type' => 'poll_choices',
    );

    $poll_settings = node_get_form_settings($form_id, 'poll_settings');
    $form['settings']['#weight'] = $poll_settings['weight'];
    $form['settings']['#form_builder'] = array(
      'element_id' => 'poll_settings',
      'element_type' => 'poll_settings',
    );
  }
}

/**
 * Implementation of hook_form_builder_save().
 */
function poll_form_builder_save(&$form, $form_type, $form_id) {
  if ($form_type == 'node') {
    $node_type = $form_id;

    $element = form_builder_get_element($form, 'poll_choices');
    $settings = array(
      'weight' => $element['#weight'],
    );
    node_set_form_settings($node_type, 'poll_choices', 'poll', $settings);

    $element = form_builder_get_element($form, 'poll_settings');
    $settings = array(
      'weight' => $element['#weight'],
    );
    node_set_form_settings($node_type, 'poll_settings', 'poll', $settings);
  }
}

/**
 * Implementation of hook_form_alter().
 *
 * @todo Merge this into poll_form_alter().
 */
function poll_node_form_alter(&$form, &$form_state, $form_id) {
  // Set properties of the poll form item based on the poll node form settings.
  if ($form_id == 'poll_node_form') {
    $choice_settings = node_get_form_settings($form['#node']->type, 'poll_choices');
    $poll_settings = node_get_form_settings($form['#node']->type, 'poll_settings');
    $form['choice_wrapper']['#weight'] = $choice_settings['weight'];
    $form['settings']['#weight'] = $poll_settings['weight'];
  }
}

/**
 * Implementation of hook_node_form_default_settings().
 *
 * Get the default settings for the poll modifications to the node form.
 */
function poll_node_form_default_settings($node_type = NULL) {
  $defaults = array();
  $defaults['poll_choices'] = array(
    'weight' => -4,
  );
    $defaults['settings'] = array(
    'weight' => -3,
  );
  return $defaults;
}
