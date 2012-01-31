<?php

/**
 * @file
 *   Default webform component callbacks for functionality related to the Form Builder.
 */

/**
 * @defgroup form-builder-webform-email-callbacks Callbacks for the Email component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_email() {
  $fields = array();

  $fields['email'] = array(
    'title' => t('E-mail'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'size',
      'key',
      'title_display',
    ),
    'default' => array(
      '#title' => t('New e-mail'),
      '#type' => 'textfield',
      '#theme_wrappers' => array('webform_element'),
      '#form_builder' => array('element_type' => 'email'),
      '#title_display' => 'before',
    ),
  );

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_email($component, $form_element) {
  $component['extra']['width'] = isset($form_element['#size']) ? $form_element['#size'] : NULL;
  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : NULL;
  $component['extra']['disabled'] = isset($form_element['#disabled']) ? $form_element['#disabled'] : FALSE;
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  return $component;
}

/**
 * @} End of "defgroup form-builder-webform-email-callbacks"
 */

/**
 * @defgroup form-builder-webform-fieldset-callbacks Callbacks for the Fieldset component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_fieldset() {
  $fields = array();

  $fields['fieldset'] = array(
    'title' => t('Fieldset'),
    'properties' => array(
      'title',
      'description',
      'collapsible',
      'collapsed',
      'key',
    ),
    'default' => array(
      '#title' => t('New fieldset'),
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    ),
  );

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_fieldset($component, $form_element) {
  $component['extra']['collapsible'] = isset($form_element['#collapsible']) ? $form_element['#collapsible'] : NULL;
  $component['extra']['collapsed'] = isset($form_element['#collapsed']) ? $form_element['#collapsed'] : NULL;
  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : '';
  return $component;
}

/**
 * @} End of "defgroup form-builder-webform-fieldset-callbacks"
 */

/**
 * @defgroup form-builder-webform-file-callbacks Callbacks for the File component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_file() {
  $defaults = _webform_defaults_file();

  $fields['file'] = array(
    'title' => t('File'),
    'properties' => array(
      'title',
      'description',
      'required',
      'key',
      'title_display',
      'webform_file_filtering',
      'webform_file_savelocation',
      'webform_file_width',
    ),
    'default' => array(
      '#title' => t('New file'),
      '#type' => 'file',
      '#theme_wrappers' => array('webform_element'),
      '#title_display' => 'before',
      '#webform_file_filtering' => $defaults['extra']['filtering'],
      '#webform_file_savelocation' => $defaults['extra']['savelocation'],
      '#webform_file_width' => $defaults['extra']['width'],
    ),
  );

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_properties_component().
 */
function _form_builder_webform_form_builder_properties_file() {
  return array(
    'webform_file_filtering' => array(
      'form' => 'form_builder_webform_property_webform_file_filtering_form',
    ),
    'webform_file_savelocation' => array(
      'form' => 'form_builder_webform_property_webform_file_savelocation_form',
    ),
    'webform_file_width' => array(
      'form' => 'form_builder_webform_property_webform_file_width_form',
    ),
  );
}

/**
 * Implements _form_builder_webform_form_builder_load_component().
 */
function _form_builder_webform_form_builder_load_file($form_element) {
  // If we are loading a file element, _webform_render_file() adds some extra
  // structure (including several element children) that we don't want here.
  // Instead, we need the element itself to represent the file component
  // directly.
  if (isset($form_element['#webform_component']['type']) && $form_element['#webform_component']['type'] == 'file') {
    $form_element['#type'] = 'file';
    unset($form_element['#theme']);
    foreach (element_children($form_element) as $key) {
      unset($form_element[$key]);
    }
    // Set the custom webform file properties based on the values stored with
    // the webform component.
    if (isset($form_element['#webform_component']['extra']['filtering'])) {
      $form_element['#webform_file_filtering'] = $form_element['#webform_component']['extra']['filtering'];
    }
    if (isset($form_element['#webform_component']['extra']['savelocation'])) {
      $form_element['#webform_file_savelocation'] = $form_element['#webform_component']['extra']['savelocation'];
    }
    if (isset($form_element['#webform_component']['extra']['width'])) {
      $form_element['#webform_file_width'] = $form_element['#webform_component']['extra']['width'];
    }
  }

  return $form_element;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_file($component, $form_element) {
  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : NULL;
  $component['extra']['disabled'] = isset($form_element['#disabled']) ? $form_element['#disabled'] : FALSE;
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  $component['extra']['filtering'] = isset($form_element['#webform_file_filtering']) ? $form_element['#webform_file_filtering'] : array();
  $component['extra']['savelocation'] = isset($form_element['#webform_file_savelocation']) ? $form_element['#webform_file_savelocation'] : NULL;
  $component['extra']['width'] = isset($form_element['#webform_file_width']) ? $form_element['#webform_file_width'] : NULL;

  return $component;
}

/**
 * Configuration form for the "webform_file_filtering" property.
 */
function form_builder_webform_property_webform_file_filtering_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_file_edit_form($element, 'webform_file_filtering', 'validation', array('validation', 'filtering'), array('extra', 'filtering'));
}

/**
 * Configuration form for the "webform_file_savelocation" property.
 */
function form_builder_webform_property_webform_file_savelocation_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_file_edit_form($element, 'webform_file_savelocation', 'default', array('extra', 'savelocation'), array('extra', 'savelocation'));
}

/**
 * Configuration form for the "webform_file_width" property.
 */
function form_builder_webform_property_webform_file_width_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_file_edit_form($element, 'webform_file_width', 'display', array('display', 'width'), array('extra', 'width'));
}

/**
 * Helper function; builds a form for editing part of a webform file component.
 *
 * The returned form is derived from a subcomponent of the _webform_edit_file()
 * form provided by the Webform module.
 *
 * @param $element
 *   A form array representing the file element whose configuration form we are
 *   building.
 * @param $property
 *   The property of $element which stores the state of portions of the webform
 *   component that this form is responsible for configuring. The property
 *   should be passed in without the leading "#".
 * @param $form_builder_property_group
 *   The Form Builder property group in which this configuration form should be
 *   displayed.
 * @param $form_nested_keys
 *   An array of nested keys representing the location of the subcomponent of
 *   the _webform_edit_file() form that this configuration form will be taken
 *   from. For example, if the part of the configuration form we are interested
 *   in is located in $form['display']['width'], where $form is the output of
 *   _webform_edit_file(), we would pass array('display', 'width') in for this
 *   parameter.
 * @param $component_nested_keys
 *   An array of nested keys representing the location of the portions of the
 *   webform component that this form is responsible for configuring. For
 *   example, if this form configures the data that is stored in
 *   $component['extra']['filtering'], where $component has the structure of
 *   the array returned by _webform_defaults_file(), we would pass
 *   array('extra', 'filtering') in for this parameter.
 *
 * @return
 *   A form array that can be used to edit the specified part of the webform
 *   file component represented by $element.
 *
 * @see _webform_edit_file()
 * @see _webform_defaults_file()
 */
function _form_builder_webform_build_file_edit_form($element, $property, $form_builder_property_group, $form_nested_keys, $component_nested_keys) {
  // The Webform module stores existing component data as part of the passed-in
  // element. If the component doesn't exist yet, initialize a default
  // component.
  $component = isset($element['#webform_component']) ? $element['#webform_component'] : _webform_defaults_file();

  // The most up-to-date configuration data stored by Form Builder for the
  // part of the component we are editing is also stored in the passed-in
  // element, and should always take precedence.
  if (isset($element["#$property"])) {
    drupal_array_set_nested_value($component, $component_nested_keys, $element["#$property"]);
  }

  // Build the entire _webform_edit_file() form based on the current state of
  // the component, and obtain the slice of it that we want.
  $form = _webform_edit_file($component);
  $form = drupal_array_get_nested_value($form, $form_nested_keys);

  // Force the form to have a consistent #tree structure so it will appear in
  // $form_state['values'] the way we want.
  _form_builder_webform_force_tree($form);

  // Indicate the Form Builder property group that this form will be displayed
  // in.
  $form['#form_builder'] = array('property_group' => $form_builder_property_group);

  // Return the form, keyed by the name of the property that is being
  // configured.
  return array($property => $form);
}

/**
 * Helper function; replaces custom form #parents with a #tree structure.
 *
 * This is a helper function to force #tree = TRUE on all parts of a form,
 * regardless of any custom #parents that were originally defined as part of
 * the form. It is used to ensure a consistent structure within
 * $form_state['values'] when the form is submitted.
 *
 * @param $form
 *   The form array to modify.
 */
function _form_builder_webform_force_tree(&$form) {
  unset($form['#parents']); 
  $form['#tree'] = TRUE; 
  foreach (element_children($form) as $key) {
    _form_builder_webform_force_tree($form[$key]);
  }
}

/**
 * @} End of "defgroup form-builder-webform-file-callbacks"
 */

/**
 * @defgroup form-builder-webform-hidden-callbacks Callbacks for the Hidden component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_hidden() {
  $fields = array();

  $fields['hidden'] = array(
    'title' => t('Hidden'),
    'properties' => array(
      'title',
      'default_value',
      'key',
    ),
    'default' => array(
      '#title' => t('Hidden field'),
      '#type' => 'hidden',
    ),
  );

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_preview_alter_component().
 */
function _form_builder_webform_form_builder_preview_alter_hidden($form_element) {
  // Convert the hidden element to a markup element before displaying it.
  $form_element['#type'] = 'markup';
  $form_element['#form_builder']['element_type'] = 'markup';
  unset($form_element['#theme']);

  // Add the 'webform_element' theme wrapper at the beginning, where it would
  // normally be.
  array_unshift($form_element['#theme_wrappers'], 'webform_element');

  // Display the title of the hidden field as regular markup.
  $form_element['#children'] = t('@title - <em>hidden field</em>', array('@title' => $form_element['#title']));
  $form_element['#title_display'] = 'none';

  // Give the element a wrapper class so that themers can recognize it
  // represents a hidden element.
  $form_element['#attributes']['class'][] = 'form-builder-preview-hidden-webform-element';
  array_unshift($form_element['#theme_wrappers'], 'container');

  return $form_element;
}

/**
 * @} End of "defgroup form-builder-webform-hidden-callbacks"
 */

/**
 * @defgroup form-builder-webform-markup-callbacks Callbacks for the Markup component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_markup() {
  $fields = array();

  $default = '';
  $default .= '<p><strong>' . t('New HTML Markup') . '</strong></p>' . "\n\n";
  $default .= '<p>' . t('Enter any text supported by available text formats.') . '</p>';

  $fields['markup'] = array(
    'title' => t('Markup'),
    'properties' => array(
      'title',
      'markup',
      'format',
      'key',
      'title_display',
    ),
    'default' => array(
      '#title' => t('Markup'),
      '#type' => 'markup',
      '#theme_wrappers' => array('webform_element'),
      '#markup' => $default,
      '#format' => NULL,
      '#title_display' => 'before',
    ),
  );

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_markup($component, $form_element) {
  $component['value'] = $form_element['#markup'];
  $component['extra']['format'] = isset($form_element['#format']) ? $form_element['#format'] : filter_default_format();
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  return $component;
}

/**
 * Implements _form_builder_webform_form_builder_preview_alter_component().
 */
function _form_builder_webform_form_builder_preview_alter_markup($form_element) {
  // Filter the markup with the appropriate text format before displaying it.
  $form_element['#markup'] = isset($form_element['#markup']) ? $form_element['#markup'] : '';
  $format = isset($form_element['#format']) ? $form_element['#format'] : filter_default_format();
  $form_element['#markup'] = check_markup(_webform_filter_values($form_element['#markup'], NULL, NULL, NULL, FALSE), $format, '', TRUE);
  if (strlen(trim($form_element['#markup'])) == 0) {
    $form_element['#markup'] = t('Empty markup field');
  }

  // This runs after drupal_pre_render_markup so the changed markup 
  // needs to be copied to #children.
  $form_element['#children'] = $form_element['#markup'];

  // Do not show the title, since it will not display in the final webform
  // either.
  $form_element['#title_display'] = 'none';

  return $form_element;
}

/**
 * @} End of "defgroup form-builder-webform-markup-callbacks"
 */

/**
 * @defgroup form-builder-webform-pagebreak-callbacks Callbacks for the Pagebreak component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_pagebreak() {
  $fields = array();

  $fields['pagebreak'] = array(
    'title' => t('Page break'),
    'properties' => array(
      'title',
      'key',
    ),
    'default' => array(
      '#title' => t('Page break'),
      '#type' => 'pagebreak',
    ),
  );

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_load_component().
 */
function _form_builder_webform_form_builder_load_pagebreak($form_element) {
  // Pagebreak components are rendered as hidden elements by webform, but
  // hidden elements do not have a #title property. So we have to convert the
  // rendered value to be used as the #title instead.
  if ($form_element['#type'] == 'pagebreak') {
    $form_element['#title'] = $form_element['#value'];
  }
  return $form_element;
}

/**
 * Implements _form_builder_webform_form_builder_preview_alter_component().
 */
function _form_builder_webform_form_builder_preview_alter_pagebreak($form_element) {
  // Convert the pagebreak element to a markup element before displaying it.
  $form_element['#type'] = 'markup';
  $form_element['#form_builder']['element_type'] = 'markup';
  unset($form_element['#theme']);

  // Add the 'webform_element' theme wrapper at the beginning, where it would
  // normally be.
  array_unshift($form_element['#theme_wrappers'], 'webform_element');

  // Display the title of the pagebreak field as regular markup.
  $form_element['#children'] = '--- ' . check_plain($form_element['#title']) . ' ---';
  $form_element['#children'] .= '<div class="description">' . t('(Fields below will be displayed on a new page)') . '</div>';
  $form_element['#title_display'] = 'none';

  // Give the element a wrapper class so that themers can recognize it
  // represents a pagebreak element.
  $form_element['#attributes']['class'][] = 'form-builder-preview-pagebreak-webform-element';
  array_unshift($form_element['#theme_wrappers'], 'container');

  return $form_element;
}

/**
 * @} End of "defgroup form-builder-webform-pagebreak-callbacks"
 */

/**
 * @defgroup form-builder-webform-select-callbacks Callbacks for the Select component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_select() {
  $fields = array();

  $fields['select'] = array(
    'title' => t('Select list'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'options',
      'multiple',
      'empty_value',
      'key',
      'title_display',
    ),
    'default' => array(
      '#title' => t('New select list'),
      '#type' => 'select',
      '#theme_wrappers' => array('webform_element'),
      '#options' => array('one' => 'one', 'two' => 'two', 'three' => 'three'),
      '#default_value' => 'one',
      '#webform' => array('type' => 'select'),
      '#title_display' => 'before',
    ),
  );

  $fields['checkboxes'] = array(
    'title' => t('Checkboxes'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'options',
      'multiple',
      'key',
      'title_display',
    ),
    'default' => array(
      '#title' => t('New checkboxes'),
      '#type' => 'checkboxes',
      '#theme_wrappers' => array('checkboxes', 'webform_element'),
      '#options' => array('one' => 'one', 'two' => 'two', 'three' => 'three'),
      '#default_value' => array('one', 'two'),
      '#multiple' => TRUE,
      '#webform' => array('type' => 'select'),
      '#title_display' => 'before',
    ),
  );

  $fields['radios'] = array(
    'title' => t('Radios'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'options',
      'key',
      'title_display',
    ),
    'default' => array(
      '#title' => t('New radios'),
      '#type' => 'radios',
      '#theme_wrappers' => array('radios', 'webform_element'),
      '#options' => array('one' => 'one', 'two' => 'two', 'three' => 'three'),
      '#default_value' => 'one',
      '#webform' => array('type' => 'select'),
      '#title_display' => 'before',
    ),
  );

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_load_component().
 */
function _form_builder_webform_form_builder_load_select($form_element) {
  if (in_array($form_element['#type'], array('radios', 'checkboxes', 'select'))) {
    $form_element['#webform']['type'] = 'select';
    if ($form_element['#type'] == 'checkboxes') {
      $form_element['#multiple'] = TRUE;
    }
    return $form_element;
  }
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_select($component, $form_element) {
  $options = '';

  foreach ($form_element['#options'] as $key => $option_value) {
    $options .= $key . '|' . $option_value . "\n";
  }

  $component['extra']['items'] = $options;
  $component['extra']['multiple'] = $form_element['#form_builder']['element_type'] == 'checkboxes';
  $component['extra']['aslist'] = $form_element['#form_builder']['element_type'] == 'select';
  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : '';
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  return $component;
}

/**
 * @} End of "defgroup form-builder-webform-select-callbacks"
 */

/**
 * @defgroup form-builder-webform-textarea-callbacks Callbacks for the Textarea component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_textarea() {
  $fields = array();

  $fields['textarea'] = array(
    'title' => t('Textarea'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'rows',
      'cols',
      'key',
      'title_display',
    ),
    'default' => array(
      '#title' => t('New textarea'),
      '#type' => 'textarea',
      '#theme_wrappers' => array('webform_element'),
      '#title_display' => 'before',
    ),
  );

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_textarea($component, $form_element) {
  $component['extra']['description'] = $form_element['#description'];
  $component['extra']['cols'] = $form_element['#cols'];
  $component['extra']['rows'] = $form_element['#rows'];
  $component['extra']['disabled'] = empty($form_element['#disabled']) ? NULL : TRUE;
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  return $component;
}

/**
 * @} End of "defgroup form-builder-webform-textarea-callbacks"
 */

/**
 * @defgroup form-builder-webform-textfield-callbacks Callbacks for the Textfield component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_textfield() {
  $fields = array();

  $fields['textfield'] = array(
    'title' => t('Textfield'),
    'properties' => array(
      'title',
      'description',
      'field_prefix',
      'field_suffix',
      'default_value',
      'required',
      'size',
      'key',
      'title_display',
    ),
    'default' => array(
      '#title' => t('New textfield'),
      '#type' => 'textfield',
      '#theme_wrappers' => array('webform_element'),
      '#title_display' => 'before',
    ),
  );

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_textfield($component, $form_element) {
  $component['extra']['width'] = isset($form_element['#size']) ? $form_element['#size'] : NULL;
  $component['extra']['maxlength'] = isset($form_element['#maxlength']) ? $form_element['#maxlength'] : NULL;
  $component['extra']['field_prefix'] = isset($form_element['#field_prefix']) ? $form_element['#field_prefix'] : NULL;
  $component['extra']['field_suffix'] = isset($form_element['#field_suffix']) ? $form_element['#field_suffix'] : NULL;
  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : NULL;
  $component['extra']['disabled'] = isset($form_element['#disabled']) ? $form_element['#disabled'] : FALSE;
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  return $component;
}

/**
 * @} End of "defgroup form-builder-webform-textfield-callbacks"
 */
