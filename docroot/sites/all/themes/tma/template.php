<?php

function tma_preprocess_html(&$vars) {
  // Add body classes
  $vars['classes_array'][] = theme_get_setting('logo') ? 'with-site-logo' : 'no-site-logo';
  $vars['classes_array'][] = (theme_get_setting('toggle_name') && filter_xss_admin(variable_get('site_name', 'Drupal'))) ? 'with-site-name' : 'no-site-name';
  $vars['classes_array'][] = (theme_get_setting('toggle_slogan') && filter_xss_admin(variable_get('site_slogan', ''))) ? 'with-site-slogan' : 'no-site-slogan';

  // Add conditional CSS for IE6 and IE7.
  drupal_add_css(drupal_get_path('theme', 'tma') . '/ie.css', array('group' => CSS_THEME, 'browsers' => array('IE' => 'lte IE 7', '!IE' => FALSE), 'preprocess' => FALSE));
}

function tma_preprocess_node(&$vars) {
  $vars['title_attributes_array']['class'] = 'node-title';

  if ($vars['teaser']) {
    $size_suffix = 'teaser';
  }
  else {
    $size_suffix = 'full';
    $vars['classes_array'][] = 'node-' . $size_suffix;
  }
  $vars['classes_array'][] = drupal_html_class('node-' . $vars['node']->type . '-' . $size_suffix);
  
  // Hide taxonomy field for teasers
  if ($vars['teaser']) {
    hide($vars['content']['field_tags']);
  }
}

function tma_preprocess_comment(&$vars) {
  $vars['title_attributes_array']['class'] = 'comment-title';
}

function tma_preprocess_block(&$vars) {
  $vars['title_attributes_array']['class'] = 'block-title';
}

function tma_preprocess_page(&$vars) {
  if (empty($vars['postamble'])) {
    // Linkback: Please do not remove this as a courtesy to the effort we have put into this theme. 
    global $base_path;
    $text = drupal_get_title();
    if (drupal_is_front_page() || (ord($text) % 2) == 0) {
      $title = 'Theme by ProsePoint Express';
      $site = 'http://www.prosepoint.net';
    }
    else {
      $title = 'Theme by ProsePoint';
      $site = 'http://www.prosepoint.org';
    }
    $text = tma_linkback_text();
    // Linkback: Please do not remove this as a courtesy to the effort we have put into this theme. 
    $vars['postamble'] = '<div style="font-size: 12px; line-height: 20px; text-align: right;"><a href="' . $site . '" title="' . $title . '"><img src="' . $base_path . drupal_get_path('theme', 'tma') . '/prosepoint_icon.png" style="vertical-align: middle;" alt="' . $text . '" title="' . $title . '" /></a></div>';
  }
  if (empty($vars['page']['footer']['#suffix'])) {
    $vars['page']['footer']['#suffix'] = '';
  }
  $vars['page']['footer']['#suffix'] .= $vars['postamble'];
}

function tma_linkback_text() {
  $options = array(
    'Online newspaper and magazine cms software', 
    'Online newspaper software',
    'Magazine and newspaper software', 
    'Content management system for newspapers and magazines', 
    'News publishing content management system', 
    'Newspaper content management system', 
    'Magazine content management system',
    'CMS for newspaper', 
    'Newspaper cms', 
    'Magazine cms', 
    'News cms',
    'Publishing software', 
    'News website publishing software', 
    'Publish your own newspaper', 
    'Make your own magazine', 
    'Publishing online newspapers',
    'Software for hosted news websites', 
    'Software to publish news website', 
    'Software to publish a newspaper',
    'Newspaper websites and web design', 
    'Newspaper web design', 
    'Magazine web design', 
    'Newspaper template for websites',
  );
  $text = drupal_get_title();
  if (empty($text)) {
    $text = variable_get('site_name', 'Drupal');
  }
  return $options[(strlen($text) + 6) % count($options)];
}

function tma_breadcrumb($variables) {
  $breadcrumb = $variables['breadcrumb'];  
  $tma_breadcrumb = theme_get_setting('tma_breadcrumb');

  if (!empty($breadcrumb) && ($tma_breadcrumb == 'yes' || ($tma_breadcrumb == 'admin' && arg(0) == 'admin'))) {
    // Provide a navigational heading to give context for breadcrumb links to
    // screen-reader users. Make the heading invisible with .element-invisible.
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';

    if (!theme_get_setting('tma_breadcrumb_home')) {
      // Optionally get rid of the homepage link
      array_shift($breadcrumb);
    }
    if (theme_get_setting('tma_breadcrumb_title')) {
      // Optionally append content title
      array_push($breadcrumb, drupal_get_title());
    }
    
    $separator = check_plain(theme_get_setting('tma_breadcrumb_separator'));
    $leading = theme_get_setting('tma_breadcrumb_leading') ? $separator : '';
    $trailing = theme_get_setting('tma_breadcrumb_trailing') ? $separator : '';

    $output .= '<div class="breadcrumb">' . $leading . implode($separator, $breadcrumb) . $trailing . '</div>';
    return $output;
  }
}
