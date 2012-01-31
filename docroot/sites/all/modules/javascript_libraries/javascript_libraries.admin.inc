<?php

/**
 * @file
 * Administrative management forms for JavaScript libraries.
 */

/**
 * Form builder function for page callback.
 */
function javascript_libraries_default_form($form, &$form_state) {
  // Add a class and attach CSS to the form for styling.
  $path = drupal_get_path('module', 'javascript_libraries');
  $form = array(
    '#attached' => array(
      'css' => array($path . '/css/javascript_libraries.admin.css',),
    ),
    '#attributes' => array(
      'class' => array('javascript-libraries'),
    ),
  );
  $libraries = array();
  // Display the libraries in fieldset groups. The groups will include any
  // library whose title matches one of the terms in the array associated with
  // each group.
  $groups = array(
    'jQuery' => array(
      'library' => array(
        'jquery',
        'jquery.once',
      ),
      'collapsed' => FALSE,
    ),
    'jQuery UI' => array(
      'library' => array(
        'ui',
        'ui.accordion',
        'ui.autocomplete',
        'ui.button',
        'ui.datepicker',
        'ui.dialog',
        'ui.slider',
        'ui.tabs',
        'effects.comprehensive',
      ),
      'collapsed' => FALSE,
    ),
  );

  // Allow other modules to add or remove available libraries.
  drupal_alter('javascript_libraries_available', $groups);

  foreach ($groups as $group => $value) {
    $form['libraries'][$group] = array();
  }
  // Get the current enabled states for the libraries
  $library_states = variable_get('javascript_libraries_drupal_libraries', array());

  // Assemble a list of all the libraries implemented by system or other modules.
  foreach (module_implements('library') as $module) {
    // drupal_get_library returns an associative array of all the libraries that
    // a module implements.
    foreach(drupal_get_library($module) as $name => $library) {
      // Only add the allowed libraries based on keys.
      foreach ($groups as $group => $value) {
        $idx = array_search($name, $value['library']);
        if ($idx !== FALSE) {
          $library['name'] = $name;
          $library['module'] = $module;
          // Change the dot characters in the name to hyphens.
          // @see http://ca.php.net/manual/en/language.variables.external.php#81080
          // Prepend the module name to the key to deal with libraries with the same
          // name registered by different modules.
          $key = $module . '-' . strtr($name, array('.' => '-'));
          // Enable the checkbox in the form for this library if it is listed
          // in the $library_states variable.
          $library['checked'] = (!empty($library_states[$key]));
          // Some libraries, like jquery, are loaded on every page already, so we
          // make them seem impossible to turn off.
          if (in_array($library['name'], array('jquery', 'jquery.once'))) {
            $library['checked'] = TRUE;
            $library['disabled'] = TRUE;
          }
          // Get the HTML for this library's row in the tableselect element.
          $form['libraries'][$group][$idx] = _javascript_libraries_build_default_row($key, $library);
          // End the foreach loop since we matched.
          break;
        }
      }
    }
  }
  // Create fieldsets for each group.
  foreach ($groups as $group => $value) {
    // Sory by key, so the order is the same as specified in the array above.
    ksort($form['libraries'][$group]);
    $form['libraries'][$group] += array(
      '#type' => 'fieldset',
      '#title' => t($group),
      '#collapsible' => TRUE,
      '#collapsed' => $value['collapsed'],
      '#theme' => 'javascript_libraries_library_fieldset',
      '#header' => array(
        array('data' => t('Enabled'), 'class' => array('checkbox')),
        array('data' => t('Name'), 'class' => array('name')),
        array('data' => t('Version'), 'class' => array('version')),
      ),
    );
  }

  // The submit button.
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Form submit function for page callback.
 */
function javascript_libraries_default_form_submit($form, &$form_state) {
  $library_states = array_filter($form_state['values']['enable']);
  // Save the values from the default library form.
  variable_set('javascript_libraries_drupal_libraries', $library_states);
  drupal_set_message(t('Your settings have been saved.'));
}

/**
 * Returns HTML for the default libraries form.
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @ingroup themeable
 */
function theme_javascript_libraries_library_fieldset($variables) {
  $form = $variables['form'];

  $rows = array();
  // Iterate through all the libraries, which are
  // children of this fieldset.
  foreach (element_children($form) as $key) {
    $library = $form[$key];
    $row = array();
    $row[] = array('class' => array('checkbox'), 'data' => drupal_render($library['enable']));
    $label = '<label';
    if (isset($library['enable']['#id'])) {
      $label .= ' for="' . $library['enable']['#id'] . '"';
    }
    $row[] = $label . '><strong>' . drupal_render($library['name']) . '</strong></label>';
    $row[] = drupal_render($library['version']);
    $rows[] = $row;
  }

  return theme('table', array('header' => $form['#header'], 'rows' => $rows));
}

/**
 * Build a table row for the default libraries table.
 */
function _javascript_libraries_build_default_row($key, $library = array()) {
  // Add in the defaults.
  $library += array(
    'disabled' => FALSE,
    'checked' => FALSE,
  );

  // Set the table row properties.
  if (!empty($library['website'])) {
    $title = l($library['title'], $library['website'], array(
      'attributes' => array(
        'title' => t('Documentation for @title', array('@title' => $library['title'])),
        'target' => '_new',
      ),
    ));
  } else {
    $title = $library['title'];
  }
  $form['name'] = array('#markup' => filter_xss_admin($title));

  $form['version'] = array('#markup' => check_plain($library['version']));

  // Build the checkbox.
  $form['enable']['#tree'] = TRUE;
  $form['enable'][$key] = array(
    '#type' => 'checkbox',
    '#default_value' => $library['checked'],
    '#return_value' => array(
      'library' => $library['name'],
      'module' => $library['module'],
    ),
  );
  if ($library['disabled']) {
    $form['enable'][$key]['#disabled'] = TRUE;
  }

  return $form;
}

/**
 * Form builder function for page callback.
 */
function javascript_libraries_custom_form($form, &$form_state) {
  // Weights range from -delta to +delta, so delta should be at least half
  // of the amount of blocks present. This makes sure all blocks in the same
  // region get an unique weight.
  $custom = variable_get('javascript_libraries_custom_libraries', array());
  $weight_delta = max(round(count($custom) / 2), 5);
  $path = drupal_get_path('module', 'javascript_libraries');
  $form = array(
    '#attached' => array(
      'css' => array($path . '/css/javascript_libraries.admin.css',),
    ),
  );
  $scope_options = array('header' => t('Head'), 'footer' => t('Footer'), 'disabled' => t('Disabled'));
  $form['page_scopes'] = array(
    '#type' => 'value',
    // Add a last region for disabled blocks.
    '#value' => $scope_options,
  );
  $form['libraries'] = array();
  $form['#header'] = array('data' => array('data' => t('Description'), 'colspan' => 2), t('Region'), array('data' => t('Weight'), 'class' => array('library-weight')), t('Source'), array('data' => t('Operations'), 'colspan' => 2));

  $form['#tree'] = TRUE;
  $block_settings = variable_get('javascript_libraries_block_settings', array());

  foreach ($custom as $key => $library) {
    $blocks = array();
    // Only show block settings when relevant:
    if ($library['scope'] == 'disabled') {
      foreach ($block_settings as $module => $deltas) {
        foreach ($deltas as $delta => $ids) {
          if (in_array($key, $ids)) {
            $module_blocks = module_invoke($module, 'block_info');
            if (!empty($module_blocks[$delta])) {
              $blocks[] = $module_blocks[$delta]['info'];
            }
          }
        }
      }
    }

    $form['libraries'][$key]['name'] = array(
      '#markup' => check_plain($library['name']),
    );

    $form['libraries'][$key]['extra'] = array(
      '#type' => 'item',
      '#description' => $blocks ?  ' ' . t('Enabled for blocks: %blocks', array('%blocks' => implode(', ', $blocks))) : '',
    );

    $form['libraries'][$key]['weight'] = array(
      '#type' => 'weight',
      '#default_value' => $library['weight'],
      '#delta' => $weight_delta,
      '#title_display' => 'invisible',
      '#title' => t('Weight for @library', array('@library' => $library['name'])),
      '#attributes' => array('class' => array('library-weight', 'library-weight-' . $library['scope'])),
    );
    $form['libraries'][$key]['scope'] = array(
      '#type' => 'select',
      '#default_value' => $library['scope'],
      '#title_display' => 'invisible',
      '#title' => t('Region for @library', array('@library' => $library['name'])),
      '#options' => $scope_options,
      '#attributes' => array('class' => array('library-region-select', 'library-region-' . $library['scope'])),
    );
    // Add brackets aroudn the disabled one.
    $form['libraries'][$key]['scope']['#options']['disabled'] = '<' . $form['libraries'][$key]['scope']['#options']['disabled'] . '>';
    $form['libraries'][$key]['type'] = array(
      '#type' => 'markup',
      '#markup' => $library['type'] == 'file' ? t('File') : t('External'),
    );
    $form['libraries'][$key]['edit'] = array(
      '#type' => 'link',
      '#title' => t('edit'),
      '#href' => 'admin/config/system/javascript-libraries/custom/' . $key . '/edit',
    );
    $form['libraries'][$key]['delete'] = array(
      '#type' => 'link',
      '#title' => t('delete'),
      '#href' => 'admin/config/system/javascript-libraries/custom/' . $key . '/delete',
   );
  }

  $form['actions'] = array(
    '#tree' => FALSE,
    '#type' => 'actions',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Default theme function for javascript_libraries_custom_form().
 *
 * @see block module drag and drop table construction.
 */
function theme_javascript_libraries_custom_form($variables) {
  $form = $variables['form'];

  $listing = array();
  $i = 0;
  foreach (element_children($form['libraries']) as $key) {
    $i++;
    $idx = $form['libraries'][$key]['weight']['#default_value'] . '.0' . $i;
    $scope = $form['libraries'][$key]['scope']['#default_value'];
    $listing[$scope][$idx]['class'] = array("draggable");
    $listing[$scope][$idx]['data'][] = drupal_render($form['libraries'][$key]['name']);
    $listing[$scope][$idx]['data'][] = drupal_render($form['libraries'][$key]['extra']);
    $listing[$scope][$idx]['data'][] = drupal_render($form['libraries'][$key]['scope']);
    $listing[$scope][$idx]['data'][] = drupal_render($form['libraries'][$key]['weight']);
    $listing[$scope][$idx]['data'][] = drupal_render($form['libraries'][$key]['type']);
    $listing[$scope][$idx]['data'][] = drupal_render($form['libraries'][$key]['edit']);
    $listing[$scope][$idx]['data'][] = drupal_render($form['libraries'][$key]['delete']);
  }
  $rows = array();
  foreach ($form['page_scopes']['#value'] as $scope => $title) {
    $populated_class = !empty($listing[$scope]) ? 'region-populated' : 'region-empty';
    $top = array(
      array('data' => array(array('data' => $title, 'colspan' => 6)), "no_striping" => TRUE, 'class' => array('region-title', 'region-list-' . $scope)),
      array('data' => array(array('data' => t('No libraries in this scope'), 'colspan' => 6)), "no_striping" => TRUE, 'class' => array('region-message', 'region-' . $scope . '-message', $populated_class)),
    );
    $rows = array_merge($rows, $top);
    if (!empty($listing[$scope])) {
      ksort($listing[$scope], SORT_NUMERIC);
      $rows = array_merge($rows, array_values($listing[$scope]));
    }
  }
  $table_attributes = array('id' => 'javascript-libraries-custom');
  if ($listing) {
    // Add table javascript if there is anything to drag-n-drop.
    drupal_add_js('misc/tableheader.js');
    drupal_add_js(drupal_get_path('module', 'javascript_libraries') . '/javascript_libraries.js');
    foreach ($form['page_scopes']['#value'] as $scope => $title) {
      drupal_add_tabledrag('javascript-libraries-custom', 'match', 'sibling', 'library-region-select', 'library-region-' . $scope, NULL, FALSE);
      drupal_add_tabledrag('javascript-libraries-custom', 'order', 'sibling', 'library-weight', 'library-weight-' . $scope);
    }
  }
  else {
    // A CSS class to hide the weight column when the table is empty.
    $table_attributes['class'] = array('empty');
  }
  return theme('table', array('header' => $form['#header'], 'rows' => $rows, 'attributes' => $table_attributes)) . drupal_render_children($form);
}

/**
 * Form submission handler for the custom libraries form.
 *
 * @see javascript_libraries_custom_form()
 */
function javascript_libraries_custom_form_submit($form, &$form_state) {
  $custom = variable_get('javascript_libraries_custom_libraries', array());

  foreach ($form_state['values']['libraries'] as $key => $library) {
    $custom[$key]['scope'] = $library['scope'];
    $custom[$key]['weight'] = $library['weight'];
  }
  variable_set('javascript_libraries_custom_libraries', $custom);
  drupal_set_message(t('The JavaScript library settings have been updated.'));
}

/**
 * Form builder function for page callback.
 */
function javascript_libraries_delete_form($form, &$form_state, $library = array()) {
  $form = array();
  $form['#library'] = $library;
  return confirm_form($form, t('Are you sure you want to delete library %name?', array('%name' => $library['name'])), 'admin/config/system/javascript-libraries/custom');
}

/**
 * Form submit function for avascript_libraries_delete_form().
 */
function javascript_libraries_delete_form_submit($form, &$form_state) {
  $library = $form['#library'];
  javascript_libraries_custom_delete($library['id']);
  drupal_set_message(t('Library %name has been removed', array('%name' => $library['name'])));
  $form_state['redirect'] = 'admin/config/system/javascript-libraries/custom';
}

/**
 * Form builder function for page callback.
 */
function javascript_libraries_edit_form($form, &$form_state, $library = array()) {

  $form['#library'] = $library;
  if (!isset($form['#library']['weight'])) {
    $form['#library']['weight'] = 5;
  }

  $form['library_type'] = array(
    '#type' => 'radios',
    '#title' => t('Source'),
    '#required' => TRUE,
    '#options' => array('external' => t('External URL'), 'file' => t('File')),
    '#default_value' => isset($library['type']) ? $library['type'] : 'external',
    '#disabled' => isset($library['type']),
  );

  $external_access = empty($library['type']) || $library['type'] == 'external';
  $form['external_url'] = array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#description' => t('Enter the full URL of a JavaScript library. URL must start with http:// or https:// and end with .js or .txt.'),
    '#states' => array(
      'visible' => array(
        ':input[name="library_type"]' => array('value' => 'external'),
      ),
    ),
    '#default_value' => isset($library['uri']) ? $library['uri'] : '',
    '#access' => $external_access,
  );


  $form['cache_external'] = array(
    '#type' => 'checkbox',
    '#title' => t('Cache script locally'),
    '#description' => t('This option only takes effect if JavaScript aggregation is enabled. You must verify that the license or terms of service for the script permit local caching and aggregation.'),
    '#default_value' => isset($library['cache']) ? $library['cache'] : FALSE,
    '#access' => $external_access,
    '#states' => array(
      'visible' => array(
        ':input[name="library_type"]' => array('value' => 'external'),
      ),
    ),
  );

  $file_access = empty($library['type']) || $library['type'] == 'file';
  $form['js_file_upload'] = array(
    '#type' => 'managed_file',
    '#title' => t('File'),
    '#description' => t('Upload a JavaScript file from your computer. It must end in .js or .txt. It will be renamed to have a .txt extension.'),
    '#upload_location' => 'public://javascript_libraries',
    '#upload_validators' => array('file_validate_extensions' => array(0 => 'js txt')),
    '#states' => array(
      'visible' => array(
        ':input[name="library_type"]' => array('value' => 'file'),
      ),
    ),
    '#default_value' => $file_access && isset($library['fid']) ? $library['fid'] : NULL,
    '#access' => empty($library['type']) || $library['type'] == 'file',
  );

  $form['scope'] = array(
    '#type' => 'select',
    '#title' => t('Region on page'),
    '#required' => TRUE,
    '#options' => array('header' => t('Head'), 'footer' => t('Footer'), 'disabled' => '<' . t('Disabled') . '>'),
    '#default_value' => isset($library['scope']) ? $library['scope'] : 'footer',
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Library description'),
    '#default_value' => isset($library['name']) ? $library['name'] : '',
    '#description' => 'Defaults to the file name or URL.',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/system/javascript-libraries/custom',
  );

  return $form;
}

/**
 * Form validate for javascript_libraries_edit_form().
 */
function javascript_libraries_edit_form_validate($form, &$form_state) {
  switch ($form_state['values']['library_type']) {
    case 'external':
      _javascript_libraries_url_validate($form, $form_state);
      break;
    case 'file':
      _javascript_libraries_file_validate($form, $form_state);
      break;
  }
  // Trim the name/description:
  $form_state['values']['name'] = trim($form_state['values']['name']);
}

function _javascript_libraries_url_validate($form, &$form_state) {
  $form_state['values']['external_url'] = trim($form_state['values']['external_url']);
  if (strlen($form_state['values']['external_url']) == 0) {
    form_set_error('external_url', t('An empty URL is invalid.'));
  }
  elseif (!javascript_libraries_valid_external_url($form_state['values']['external_url'])) {
    form_set_error('external_url', t('This URL does not start with http:// or does not end with ".js" or ".txt".'));
  }
}

function _javascript_libraries_file_validate($form, &$form_state) {
  if (empty($form_state['values']['js_file_upload'])) {
    form_set_error('js_file_upload', 'File field is required when adding a file.');
  }
}

/**
 * Form submit for javascript_libraries_edit_form().
 */
function javascript_libraries_edit_form_submit($form, &$form_state) {
  switch ($form_state['values']['library_type']) {
    case 'external':
      if (empty($form['#library']['id'])) {
        // New URL
        $form['#library']['id'] = 'ext-' . db_next_id();
      }
      $custom = variable_get('javascript_libraries_custom_libraries', array());
      if (strlen($form_state['values']['name']) == 0) {
        $parts = explode('/', $form_state['values']['external_url']);
        $form_state['values']['name'] = '... /' . end($parts);
      }
      $custom[$form['#library']['id']] = array(
        'id' => $form['#library']['id'],
        'type' => 'external',
        'scope' => $form_state['values']['scope'],
        'name' => $form_state['values']['name'],
        'weight' => $form['#library']['weight'],
        'uri' => $form_state['values']['external_url'],
        'cache' => $form_state['values']['cache_external'],
      );
      variable_set('javascript_libraries_custom_libraries', $custom);
      break;
    case 'file':
      _javascript_libraries_file_submit($form, $form_state);
      // Change query-strings on css/js files to enforce reload for all users.
      javascript_libraries_js_cache_clear();
      break;
  }
  drupal_set_message('Your library has been added. Please configure the region and weight.');
  $form_state['redirect'] = 'admin/config/system/javascript-libraries/custom';
}

function _javascript_libraries_file_submit($form, &$form_state) {
  $file = file_load($form_state['values']['js_file_upload']);
  $file->status = FILE_STATUS_PERMANENT;
  // Make the file permanent.
  file_save($file);
  file_usage_add($file, 'javascript_libraries', 'javascript_libraries', $file->fid);
  $custom = variable_get('javascript_libraries_custom_libraries', array());
  $id = 'file-' . $file->fid;
  $custom[$id] = array(
    'type' => 'file',
    'scope' => $form_state['values']['scope'],
    'name' => strlen($form_state['values']['name']) ? $form_state['values']['name'] : $file->filename,
    'weight' => $form['#library']['weight'],
    'id' => $id,
    'fid' => $file->fid,
    'uri' => $file->uri,
  );
  variable_set('javascript_libraries_custom_libraries', $custom);

  if (isset($form['#library']['fid']) && $form['#library']['fid'] != $file->fid) {
    // Replacement file. The id and file have changed, so delete the old one.
    javascript_libraries_custom_delete($form['#library']['id']);
  }
}

