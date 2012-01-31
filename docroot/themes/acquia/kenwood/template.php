<?php

/**
 * Implement hook_preprocess_html().
 */
function kenwood_preprocess_html(&$vars) {
  $vars['classes_array'][] = 'theme-markup-2';
  $vars['classes_array'][] = _kenwood_get_layout();
}

/**
 * Implements hook_html_head_alter().
 */
function kenwood_html_head_alter(&$head_elements) {
  // If the theme's info file contains the custom theme setting
  // default_favicon_path, change the favicon <link> tag to reflect that path.
  if (($default_favicon_path = theme_get_setting('default_favicon_path')) && theme_get_setting('default_favicon')) {
    $favicon_url = file_create_url(path_to_theme() . '/' . $default_favicon_path);
  }
  else {
    if (module_exists('gardens_misc')) {
      $favicon_url = file_create_url(drupal_get_path('module', 'gardens_misc') . '/images/gardens.ico');
    }
  }
  if (!empty($favicon_url)) {
    $favicon_mimetype = file_get_mimetype($favicon_url);
    foreach ($head_elements as &$element) {
      if (isset($element['#attributes']['rel']) && $element['#attributes']['rel'] == 'shortcut icon') {
	$element['#attributes']['href'] = $favicon_url;
	$element['#attributes']['type'] = $favicon_mimetype;
      }
    }
  }
}

/**
* Implements hook_preprocess_page().
*/

function kenwood_preprocess_page(&$variables) {
  $is_front = $variables['is_front'];
  // Adjust the html element that wraps the site name. h1 on front page, p on other pages
  $variables['wrapper_site_name_prefix'] = ($is_front ? '<h1' : '<p');
  $variables['wrapper_site_name_prefix'] .= ' id="site-name"';
  $variables['wrapper_site_name_prefix'] .= ' class="site-name'.($is_front ? ' site-name-front' : '').'"';
  $variables['wrapper_site_name_prefix'] .= '>';
  $variables['wrapper_site_name_suffix'] = ($is_front ? '</h1>' : '</p>');
  // If the theme's info file contains the custom theme setting
  // default_logo_path, set the $logo variable to that path.
  $default_logo_path = theme_get_setting('default_logo_path');
  if (!empty($default_logo_path) && theme_get_setting('default_logo')) {
    $variables['logo'] = file_create_url(path_to_theme() . '/' . $default_logo_path);
  }
  else {
    $variables['logo'] = null;
  }
  
  //Arrange the elements of the main content area (content and sidebars) based on the layout class
  $layoutClass = _kenwood_get_layout();
  $layout = substr(strrchr($layoutClass, '-'), 1); //Get the last bit of the layout class, the 'abc' string
  
  $contentPos = strpos($layout, 'c');
  $sidebarsLeft = substr($layout,0,$contentPos);
  $sidebarsRight = strrev(substr($layout,($contentPos+1))); // Reverse the string so that the floats are correct.
  
  $sidebarsHidden = ''; // Create a string of sidebars that are hidden to render and then display:none
  if(stripos($layout, 'a') === false) { $sidebarsHidden .= 'a'; }
  if(stripos($layout, 'b') === false) { $sidebarsHidden .= 'b'; }
  
  $variables['sidebars']['left'] = str_split($sidebarsLeft);
  $variables['sidebars']['right'] = str_split($sidebarsRight);
  $variables['sidebars']['hidden'] = str_split($sidebarsHidden);
}

/**
 * Implement hook_preprocess_block().
 */
function kenwood_preprocess_block(&$vars) {
  $vars['content_attributes_array']['class'][] = 'content';
  //$vars['content_attributes_array']['class'][] = !empty($vars['block']->subject) ? 'content-with-title' : 'content-without-title';
}

/**
 * Retrieves the value associated with the specified key from the current theme.
 * If the key is not found, the specified default value will be returned instead.
 *
 * @param <string> $key
 *   The name of the key.
 * @param <mixed> $default
 *   The default value, returned if the property key is not found in the current
 *   theme.
 * @return <mixed>
 *   The value associated with the specified key, or the default value.
 */
function _kenwood_variable_get($key, $default) {
  global $theme;
  $themes_info =& drupal_static(__FUNCTION__);
  if (!isset($themes_info[$theme])) {
    $themes_info = system_get_info('theme');
  }

  $value = $themes_info[$theme];
  foreach (explode('/', $key) as $part) {
    if (!isset($value[$part])) {
      return $default;
    }
    $value = $value[$part];
  }
  return $value;
}

/**
 * Returns the name of the layout class associated with the current path.  The
 * layout name is used as a body class, which causes the page to be styled
 * with the corresponding layout.  This function makes it possible to use
 * different layouts on various pages of a site.
 *
 * @return <string>
 *   The name of the layout associated with the current page.
 */
function _kenwood_get_layout() {
  $layout_patterns = _kenwood_variable_get('layout', array('<global>' => 'body-layout-fixed-abc'));
  $global_layout = $layout_patterns['<global>'];
  unset($layout_patterns['<global>']);

  $alias_path = drupal_get_path_alias($_GET['q']);
  $path = $_GET['q'];
  foreach ($layout_patterns as $pattern => $layout) {
    if (drupal_match_path($alias_path, $pattern) ||
        drupal_match_path($path, $pattern)) {
      return $layout;
    }
  }
  return $global_layout;
}

/**
 * Implements hook_node_view_alter().
 */
function kenwood_node_view_alter(&$build) {
  if (isset($build['links']) && isset($build['links']['comment']) &&
    isset($build['links']['comment']['#attributes']) &&
    isset($build['links']['comment']['#attributes']['class'])) {
    $classes = $build['links']['comment']['#attributes']['class'];
    array_push($classes, 'actions');
    $build['links']['comment']['#attributes']['class'] = $classes;
  }
}

/**
 * Implements hook_preprocess_forum_topic_list
 */

function kenwood_preprocess_forum_topic_list(&$vars) {
  // Recreate the topic list header
  $list = array(
    array('data' => t('Topic'), 'field' => 'f.title'),
    array('data' => t('Replies'), 'field' => 'f.comment_count'),
    array('data' => t('Created'), 'field' => 't.created'),
    array('data' => t('Last reply'), 'field' => 'f.last_comment_timestamp'),
  );
  
  $ts = tablesort_init($list);
  $header = '';
  foreach ($list as $cell) {
    $cell = tablesort_header($cell, $list, $ts);
    $header .= _theme_table_cell($cell, TRUE);
  }
  $vars['header'] = $header;
}

/*
 * Implements hook_preprocess_comment_wrapper().
 */
function kenwood_preprocess_comment_wrapper(&$vars) {
  // If labels for the template haven't been defined (such as in the
  // comment_goodness module) define them here.
  if (empty($vars['labels']['form_label'])) {
    $vars['labels']['form_label'] = t('Post new comment');
  }
  if (empty($vars['labels']['section_label'])) {
    $vars['labels']['section_label'] = t('Comments');
  }
}

/*
 * Implements hook_preprocess_media_gallery_license().
 */
function kenwood_preprocess_media_gallery_license(&$vars) {

}
