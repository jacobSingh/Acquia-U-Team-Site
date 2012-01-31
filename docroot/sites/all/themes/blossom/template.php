<?php

/**
 * Override or insert variables into the html template.
 */
function blossom_preprocess_html(&$vars) {
  $theme_path = path_to_theme();
  // IE7+
  drupal_add_css($theme_path . '/css/ie/ie-all.css', array('group' => CSS_THEME, 'media' => 'screen, handheld, projection, tv', 'browsers' => array('IE' => 'IE', '!IE' => FALSE), 'preprocess' => FALSE,));
  drupal_add_css($theme_path . '/css/ie/ie-7.css', array('group' => CSS_THEME, 'media' => 'screen, handheld, projection, tv', 'browsers' => array('IE' => 'IE 7', '!IE' => FALSE), 'preprocess' => FALSE,));
  drupal_add_css($theme_path . '/css/ie/ie-8.css', array('group' => CSS_THEME, 'media' => 'screen, handheld, projection, tv', 'browsers' => array('IE' => 'IE 8', '!IE' => FALSE), 'preprocess' => FALSE,));
  drupal_add_css($theme_path . '/css/ie/ie-lte-8.css', array('group' => CSS_THEME, 'media' => 'screen, handheld, projection, tv', 'browsers' => array('IE' => 'lte IE 8', '!IE' => FALSE), 'preprocess' => FALSE,));
  drupal_add_css($theme_path . '/css/ie/ie-9.css', array('group' => CSS_THEME, 'media' => 'screen, handheld, projection, tv', 'browsers' => array('IE' => 'IE 9', '!IE' => FALSE), 'preprocess' => FALSE,));
}

/**
 * Implements theme_webform_results_analysis().
 */
function blossom_webform_results_analysis($variables) {
  $node = $variables['node'];
  $data = $variables['data'];
  $sids = $variables['sids'];
  $analysis_component = $variables['component'];

  $rows = array();
  $question_number = 0;
  $single = isset($analysis_component);

  $header = array(
    $single ? $analysis_component['name'] : t('Q'),
    array('data' => $single ? '&nbsp;' : t('responses'), 'colspan' => '10')
  );

  foreach ($data as $cid => $row_data) {
    $question_number++;

    if (is_array($row_data)) {
      $row = array();
      if (!$single) {
        $row[] = array('data' => '<strong>' . $question_number . '</strong>', 'valign' => 'top');

        $subhead = array(
          array('data' => '<strong>' . check_plain($node->webform['components'][$cid]['name']) . '</strong>', 'colspan' => '2')
        );

        $data = theme('table', array('header' => $subhead, 'rows' => $row_data, 'sticky' => FALSE, 'attributes' => array('class' => array('webform-response'))));

        $row[] = array('data' => $data);
      }
      $rows[] = $row;
    }
  }

  if (count($rows) == 0) {
    $rows[] = array(array('data' => t('There are no submissions for this form. <a href="!url">View this form</a>.', array('!url' => url('node/' . $node->nid))), 'colspan' => 20));
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}
