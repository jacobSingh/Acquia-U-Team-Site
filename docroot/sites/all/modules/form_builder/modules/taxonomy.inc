<?php

/**
 * @file
 *   Form builder implementation for taxonomy.module.
 */

/**
 * Implementation of hook_form_builder_types().
 */
function taxonomy_form_builder_types() {
  $fields = array();

  $vocabularies = taxonomy_get_vocabularies();
  $terms = array();

  // If no taxonomies are specified, do not allow the field to be added.
  if (empty($vocabularies)) {
    return array();
  }

  $first_vocab = reset($vocabularies);
  $vocabularies = array($first_vocab->vid => $first_vocab);
  $settings = module_invoke('taxonomy', 'node_form_default_settings');;
  $default_form = taxonomy_fieldset($vocabularies, $terms, $settings['taxonomy']);
  $default_form['#taxonomy_vocabularies'] = array_keys($vocabularies);

  $fields['taxonomy'] = array(
    'title' => t('Taxonomy'),
    'properties' => array(
      'taxonomy_vocabularies',
    ),
    'default' => $default_form,
    'unique' => TRUE,
    'palette_group' => 'special',
  );

  return array(
    'node' => $fields,
    'example' => $fields,
  );
}

/**
 * Implementation of hook_form_builder_properties().
 */
function taxonomy_form_builder_properties($form_type) {
  if ($form_type == 'node') {
    return array(
      'taxonomy_vocabularies' => array(
        'form' => 'taxonomy_vocabularies_configuration',
      ),
    );
  }
}

/**
 * Form builder property form callback.
 */
function taxonomy_vocabularies_configuration(&$form_state, $form_type, $element) {
  $form = array();

  $vocabularies = taxonomy_get_vocabularies();
  $options = array();
  foreach ($vocabularies as $vid => $vocabulary) {
    $options[$vid] = $vocabulary->name;
  }

  $form['taxonomy_vocabularies'] = array(
    '#title' => t('Enabled vocabularies'),
    '#type' => 'checkboxes',
    '#default_value' => $element['#taxonomy_vocabularies'],
    '#options' => $options,
    '#required' => TRUE,
    '#description' => t('Configure taxonomy element properties or add new vocabularies by visiting the <a href="!url">Taxonomy administration section</a>. If no vocabularies are desired for this form, remove the taxonomy element entirely.', array('!url' => url('admin/content/taxonomy'))),
  );

  return $form;
}

/**
 * Implementation of hook_form_builder_preview_alter().
 *
 * Update the taxonomy element with the new vocabularies.
 */
function taxonomy_form_builder_preview_alter(&$element, $form_type, $form_id) {
  if ($element['#form_builder']['element_id'] == 'taxonomy') {
    // Remove current taxonomies from the element.
    foreach (element_children($element) as $key) {
      if (is_numeric($key)) {
        unset($element[$key]);
      }
    }

    $vocabularies = array_intersect_key(taxonomy_get_vocabularies(), array_flip($element['#taxonomy_vocabularies']));
    $terms = array();
    $settings = array(
      'weight' => $element['#weight'],
    );
    $form = taxonomy_fieldset($vocabularies, $terms, $settings);
    $element = array_merge($form, $element);
  }
}

/**
 * Implementation of hook_form_builder_load_alter().
 */
function taxonomy_form_builder_load_alter(&$form, $form_type, $form_id) {
  if ($form_type == 'node') {
    $settings = node_get_form_settings($form_id, 'taxonomy');
    $vocabularies = taxonomy_get_vocabularies($form['#node']->type);
    $terms = isset($form['#node']->terms) ? $form['#node']->terms : array();

    if (count($vocabularies)) {
      $form['taxonomy'] = taxonomy_fieldset($vocabularies, $terms, $settings);
      $form['taxonomy']['#form_builder'] = array(
        'element_id' => 'taxonomy',
        'element_type' => 'taxonomy',
      );
      $form['taxonomy']['#taxonomy_vocabularies'] = array_keys($vocabularies);
    }
  }
}

/**
 * Implementation of hook_form_alter().
 *
 * @todo Merge this into taxonomy_form_alter().
 */
function taxonomy_node_form_alter(&$form, &$form_state, $form_id) {
  // Set properties of the menu form item based on the menu node form settings.
  if (isset($form['#node']) && $form['#node']->type .'_node_form' == $form_id) {
    if (isset($form['taxonomy'])) {
      $settings = node_get_form_settings($form['#node']->type, 'taxonomy');
      $form['taxonomy']['#weight'] = $settings['weight'];
    }
  }
}
/**
 * Implementation of hook_form_builder_save().
 */
function taxonomy_form_builder_save(&$form, $form_type, $form_id) {
  if ($form_type == 'node') {
    $element = form_builder_get_element($form, 'taxonomy');
    $vocabularies = taxonomy_get_vocabularies();
    $node_type = $form_id;

    if ($element) {
      $settings = array('weight' => $element['#weight']);
      variable_set('taxonomy_settings_' . $node_type, $settings);
      $enabled_vocabularies = drupal_map_assoc($element['#taxonomy_vocabularies']);
      foreach ($vocabularies as $vid => $vocabulary) {
        $vocabulary = (array) $vocabulary;
        if (isset($enabled_vocabularies[$vid]) && !isset($vocabular['nodes'][$node_type])) {
          $vocabulary['nodes'][$node_type] = $node_type;
          taxonomy_save_vocabulary($vocabulary);
        }
        elseif (!isset($enabled_vocabularies[$vid]) && isset($vocabulary['nodes'][$node_type])) {
          unset($vocabulary['nodes'][$node_type]);
          taxonomy_save_vocabulary($vocabulary);
        }
      }
    }
    else {
      variable_del('taxonomy_settings_' . $node_type);
      foreach ($vocabularies as $vid => $vocabulary) {
        $vocabulary = (array) $vocabulary;
        if (isset($vocabulary['nodes'][$node_type])) {
          unset($vocabulary['nodes'][$node_type]);
          taxonomy_save_vocabulary($vocabulary);
        }
      }
    }
  }
}

/**
 * Implementation of hook_node_form_default_settings().
 *
 * Get the settings for the taxonomy fieldset on the node form.
 */
function taxonomy_node_form_default_settings($type = NULL) {
  $defaults = array();
  $defaults['taxonomy'] = array(
    'weight' => -3,
  );
  return $defaults;
}

/**
 * A straight copy/paste of taxonomy_form_alter(). Get the taxonomy node form.
 *
 * @todo Put this function directly in taxonomy.module, abstracting it from
 * taxonomy_form_alter().
 */
function taxonomy_fieldset($vocabularies, $terms, $settings) {
  $form = array();

  if (!variable_get('taxonomy_override_selector', FALSE)) {
    foreach ($vocabularies as $vocabulary) {
      if ($vocabulary->tags) {
        if (isset($form_state['node_preview'])) {
          // Typed string can be changed by the user before preview,
          // so we just insert the tags directly as provided in the form.
          $typed_string = $terms['tags'][$vocabulary->vid];
        }
        else {
          $typed_string = taxonomy_implode_tags($terms, $vocabulary->vid) . (array_key_exists('tags', $terms) ? $terms['tags'][$vocabulary->vid] : NULL);
        }
        if ($vocabulary->help) {
          $help = $vocabulary->help;
        }
        else {
          $help = t('A comma-separated list of terms describing this content. Example: funny, bungee jumping, "Company, Inc.".');
        }
        $form['tags'][$vocabulary->vid] = array('#type' => 'textfield',
          '#title' => $vocabulary->name,
          '#description' => $help,
          '#required' => $vocabulary->required,
          '#default_value' => $typed_string,
          '#autocomplete_path' => 'taxonomy/autocomplete/'. $vocabulary->vid,
          '#weight' => $vocabulary->weight,
          '#maxlength' => 255,
        );
      }
      else {
        // Extract terms belonging to the vocabulary in question.
        $default_terms = array();
        foreach ($terms as $term) {
          // Free tagging has no default terms and also no vid after preview.
          if (isset($term->vid) && $term->vid == $vocabulary->vid) {
            $default_terms[$term->tid] = $term;
          }
        }
        $form[$vocabulary->vid] = taxonomy_form($vocabulary->vid, array_keys($default_terms), $vocabulary->help);
        $form[$vocabulary->vid]['#weight'] = $vocabulary->weight;
        $form[$vocabulary->vid]['#required'] = $vocabulary->required;
      }
    }
    if (!empty($form) && is_array($form)) {
      if (count($form) > 1) {
        // Add fieldset only if form has more than 1 element.
        $form += array(
          '#type' => 'fieldset',
          '#title' => t('Vocabularies'),
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
        );
      }
      $form['#weight'] = $settings['weight'];
      $form['#tree'] = TRUE;
    }
  }

  return $form;
}
