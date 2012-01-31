<?php

/**
 * @file
 * Integration of module defined fields with Field UI.
 */

/**
 * Alters the Manage Fields form.
 *
 * In several places, Field API and UI assume that a field doesn't exist if
 * there are no instances of it, and Field API deletes the field when the last
 * instance is deleted, resulting in no UI for adding a module defined field to
 * its first bundle.
 *
 * This function adds uninstantiated module defined fields to the
 * '_add_existing_field' dropdown.
 */
function _modulefield_field_overview_form_alter(&$form, &$form_state) {
  // @todo If a bundle already has all of the existing fields that Field API
  //   knows about, then this UI element doesn't exist, and this module fails to
  //   make the module defined fields available to this bundle. However, for
  //   most sites, this condition is rare.
  if (isset($form['fields']['_add_existing_field']['field_name'])) {
    $element = &$form['fields']['_add_existing_field']['field_name'];
    $entity_type = $form['#entity_type'];
    $bundle = $form['#bundle'];
    $field_types = field_info_field_types();

    $js_fields = array();
    foreach (modulefield_info() as $field_name => $field) {
      // Same logic as in field_ui_existing_field_options() for determining if a
      // field is addable to a bundle.
      if (!isset($element['#options'][$field_name])
        && empty($field['locked'])
        && !field_info_instance($entity_type, $field_name, $bundle)
        && (empty($field['entity_types']) || in_array($entity_type, $field['entity_types']))
        && empty($field_types[$field['type']]['no_ui'])
        // Additional logic for supporting 'no_ui' on fields, not just field
        // types.
        && empty($field['no_ui'])
      ) {
        // Add the field as an option, as in field_ui_existing_field_options().
        $element['#options'][$field_name] = t('@type: @field (@label)', array(
          '@type' => $field_types[$field['type']]['label'],
          '@field' => $field_name,
          '@label' => $field['instance']['label'],
        ));
      }

      // Add settings for the update selects behavior. We do this even for
      // fields that already existed in $element['#options'], because
      // field_ui_field_overview_form() fails to set the desired default label.
      $js_fields[$field_name] = array('label' => $field['instance']['label'], 'type' => $field['type'], 'widget' => NULL);
    }
    asort($element['#options']);
    $form['#attached']['js'][] = array(
      'type' => 'setting',
      'data' => array('fields' => $js_fields),
    );

    // If a module defined field that Field API doesn't yet know about is
    // selected, we need to create it before letting the Field UI handler add it
    // to the bundle.
    array_unshift($form['#submit'], '_modulefield_field_overview_form_submit');
  }

  // Allow module-defined fields to specify a 'no_ui' option, just as field
  // types can. Don't show these fields on the form.
  foreach (modulefield_info() as $field_name => $field) {
    if (!empty($field['no_ui']) && isset($form['fields'][$field_name])) {
      $form['fields'][$field_name]['#access'] = FALSE;
    }
  }
}

/**
 * Alters the Manage Display form.
 */
function _modulefield_display_overview_form_alter(&$form, &$form_state) {
  // Allow module-defined fields to specify a 'no_ui' option, just as field
  // types can. Don't show these fields on the form.
  foreach (modulefield_info() as $field_name => $field) {
    if (!empty($field['no_ui']) && isset($form['fields'][$field_name])) {
      $form['fields'][$field_name]['#access'] = FALSE;
    }
  }
}

/**
 * Form submit handler prepended to the field overview form.
 *
 * If a not-yet-created module defined field was selected, this handler creates
 * it, so that the Field UI handler can successfully add it to the bundle.
 *
 * @see _modulefield_field_overview_form_alter()
 */
function _modulefield_field_overview_form_submit($form, &$form_state) {
  $field_name = !empty($form_state['values']['fields']['_add_existing_field']['field_name']) ? $form_state['values']['fields']['_add_existing_field']['field_name'] : NULL;
  if (isset($field_name) && modulefield_info($field_name) && !modulefield_field_exists($field_name)) {
    modulefield_create_field($field_name);
  }
}
