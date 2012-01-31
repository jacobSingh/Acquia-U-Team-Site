<?php

/**
 * @file
 *   Form builder implementation for menu.module.
 */

/**
 * Implementation of hook_form_builder_types().
 */
function menu_form_builder_types() {
  $fields = array();

  // Make a default menu item to pass into the menu form.
  $item = array(
    'mlid' => 0,
    'plid' => 0,
    'weight' => 0,
    'link_title' => '',
    'module' => '',
    'hidden' => NULL,
    'has_children' => NULL,
    'customized' => NULL, 
    'options' => NULL,
    'expanded' => NULL,
    'parent_depth_limit' => NULL,
    'menu_name' => NULL,
  );
  $default_settings = module_invoke('menu', 'node_form_default_settings');

  $fields['menu_settings'] = array(
    'title' => t('Menu'),
    'properties' => array(
      'collapsible',
      'collapsed',
    ),
    'default' => menu_fieldset($item, $default_settings['menu']),
    'unique' => TRUE,
    'configurable' => TRUE,
    'removable' => TRUE,
    'palette_group' => 'special',
  );

  return array(
    'node' => $fields,
    'example' => $fields,
  );
}

/**
 * Implementation of hook_form_builder_load_alter().
 */
function menu_form_builder_load_alter(&$form, $form_type, $form_id) {
  if ($form_type == 'node') {
    $menu_settings = node_get_form_settings($form_id, 'menu');

    if ($menu_settings['enabled']) {
      $form['menu'] = menu_fieldset($form['#node']->menu, $menu_settings);
      $form['menu']['#form_builder'] = array(
        'element_id' => 'menu_settings',
        'element_type' => 'menu_settings',
      );
    }
  }
}

/**
 * Implementation of hook_form_builder_save().
 */
function menu_form_builder_save(&$form, $form_type, $form_id) {
  if ($form_type == 'node') {
    $node_type = $form_id;
    $element = form_builder_get_element($form, 'menu_settings');
    if ($element) {
      $settings = array(
        'enabled' => TRUE,
        'collapsible' => !empty($element['#collapsible']),
        'collapsed' => !empty($element['#collapsible']) && !empty($element['#collapsed']),
        'weight' => $element['#weight'],
      );
    }
    else {
      $settings = array(
        'enabled' => FALSE,
      );
    }
    node_set_form_settings($node_type, 'menu', 'menu', $settings);
  }
}

/**
 * Implementation of hook_form_alter().
 *
 * @todo Merge this into menu_form_alter().
 */
function menu_node_form_alter(&$form, &$form_state, $form_id) {
  // Set properties of the menu form item based on the menu node form settings.
  if (isset($form['#node']) && $form['#node']->type .'_node_form' == $form_id) {
    $settings = node_get_form_settings($form['#node']->type, 'menu');
    if ($settings['enabled']) {
      $form['menu']['#collapsible'] = $settings['collapsible'];
      $form['menu']['#collapsed'] = $settings['collapsed'];
      $form['menu']['#weight'] = $settings['weight'];
    }
    else {
      $form['menu']['#access'] = FALSE;
    }
  }
}

/**
 * Implementation of hook_node_form_default_settings().
 *
 * Get the default settings for the menu modifications to the node form.
 */
function menu_node_form_default_settings($node_type = NULL) {
  $defaults = array();
  $defaults['menu'] = array(
    'enabled' => TRUE,
    'collapsible' => TRUE,
    'collapsed' => TRUE,
    'weight' => -2,
  );
  return $defaults;
}

/**
 * A straight copy/paste of menu_form_alter(). Get the menu node form.
 *
 * @param $item
 *   The contents of $node->menu.
 * @param $menu_settings
 *   Settings for the menu form.
 *
 * @todo Put this function directly in menu.module, abstracting it from
 * menu_form_alter().
 */
function menu_fieldset($item, $menu_settings = array()) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Menu settings'),
    '#access' => user_access('administer menu'),
    '#collapsible' => $menu_settings['collapsible'],
    '#collapsed' => $menu_settings['collapsible'],
    '#tree' => TRUE,
    '#attributes' => array('class' => 'menu-item-form'),
    '#weight' => $menu_settings['weight'],
  );

  if ($item['mlid']) {
    // There is an existing link.
    $form['delete'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete this menu item.'),
    );
  }
  if (!$item['link_title']) {
    $form['#collapsed'] = TRUE;
  }

  foreach (array('mlid', 'module', 'hidden', 'has_children', 'customized', 'options', 'expanded', 'hidden', 'parent_depth_limit') as $key) {
    $form[$key] = array('#type' => 'value', '#value' => $item[$key]);
  }
  $form['#item'] = $item;

  $form['link_title'] = array('#type' => 'textfield',
    '#title' => t('Menu link title'),
    '#default_value' => $item['link_title'],
    '#description' => t('The link text corresponding to this item that should appear in the menu. Leave blank if you do not wish to add this post to the menu.'),
    '#required' => FALSE,
  );
  // Generate a list of possible parents (not including this item or descendants).
  $options = menu_parent_options(menu_get_menus(), $item);
  $default = $item['menu_name'] .':'. $item['plid'];
  if (!isset($options[$default])) {
    $default = 'primary-links:0';
  }
  $form['parent'] = array(
    '#type' => 'select',
    '#title' => t('Parent item'),
    '#default_value' => $default,
    '#options' => $options,
    '#description' => t('The maximum depth for an item and all its children is fixed at !maxdepth. Some menu items may not be available as parents if selecting them would exceed this limit.', array('!maxdepth' => MENU_MAX_DEPTH)),
    '#attributes' => array('class' => 'menu-title-select'),
  );
  $form['#submit'][] = 'menu_node_form_submit';

  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#delta' => 50,
    '#default_value' => $item['weight'],
    '#description' => t('Optional. In the menu, the heavier items will sink and the lighter items will be positioned nearer the top.'),
  );

  return $form;
}
