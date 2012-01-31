<?php
// $Id: mailing_list.admin.inc,v 1.3.4.4 2010/07/13 20:40:11 JacobSingh Exp $

/**
 * @file
 * Mailing list admin UI.
 */

/**
 * Menu callback; displays all mailing lists in a table.
 */
function mailing_list_lists() {
  $header = array(
    array('data' => t('Name'), 'field' => 'name', 'sort' => 'asc'),
    array('data' => t('Operations'), 'colspan' => '5')
  );

  $lists = mailing_list_get();
  $rows = array();
  $destination = drupal_get_destination();
  foreach ($lists as $mlid => $list) {
    $row = array(l(check_plain($list->name), "admin/structure/mailing-list/$list->mlid"), l(t('list e-mails'), "admin/structure/mailing-list/$list->mlid"), l(t('import e-mails'), "admin/structure/mailing-list/$list->mlid/import", array('query' => $destination)), l(t('export list'), "admin/structure/mailing-list/$list->mlid/export", array('query' => $destination)), l(t('rename list'), "admin/structure/mailing-list/$list->mlid/edit", array('query' => $destination)), l(t('delete list'), "admin/structure/mailing-list/$list->mlid/delete", array('query' => $destination)));
    $rows[] = $row;
  }

  if (empty($rows)) {
    $empty_message = t('No mailing lists found.');
    $rows[] = array(array('data' => $empty_message, 'colspan' => 6));
  }
  // @todo: Perhaps change to $build arrays or even table select?
  $output = theme('table', array('header' => $header, 'rows' => $rows));

  return $output;
}

/**
 * Menu callback; displays all e-mails for the specified mailing list in a
 * table.  Also routes to delete operation if selected.
 *
 * Hack inspired by user.module
 */
function mailing_list_emails_list($list) {
  if (!empty($_POST['mails']) && isset($_POST['operation']) && ($_POST['operation'] == 'delete')) {
    $build['mailing_list_email_delete_multiple_confirm'] = drupal_get_form('mailing_list_email_delete_multiple_confirm_form');
  } else {
    $build['mailing_list_emails_form'] = drupal_get_form('mailing_list_emails_form', $list);
  }
  return $build;
}

/**
 * Menu callback; displays all e-mails for the specified mailing list in a
 * table.
 *
 */
function mailing_list_emails_form($form, &$form_state, $list = NULL) {
  if (empty($list)) {
    return;
  }
  else {
    drupal_set_title(check_plain($list->name));
  }

  $header = array(
    'mail' => array('data' => t('E-mail'), 'field' => 'mle.mail', 'sort' => 'asc'),
    'name' => array('data' => t('Name'), 'field' => 'mle.name'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('mailing_list_emails', 'mle')
    ->condition('mlid', $list->mlid);

  $count_query = clone $query;
  $count_query->addExpression('COUNT(mle.eid)');

  $query = $query->extend('PagerDefault')->extend('TableSort');
  $query
    ->fields('mle')
    ->limit(variable_get('mailing_list_limit', MAILING_LIST_LIMIT))
    ->orderByHeader($header)
    ->setCountQuery($count_query);
  $result = $query->execute();

  $rows = array();
  $destination = drupal_get_destination();
  foreach ($result as $data) {
    // Edit link is broken, see http://drupal.org/node/704564
    $rows[$data->eid] = array(
      'mail' => check_plain($data->mail),
      'name' => (!empty($data->name) ? check_plain($data->name) : theme('placeholder', t('none'))),
      'operations' => l(t('edit'), "admin/structure/mailing-list/$list->mlid/$data->eid", array('query' => $destination)), l(t('delete'), "admin/structure/mailing-list/$list->mlid/$data->eid/delete", array('query' => $destination))
    );
  }

  // Until we have multiple operations, hiding this.
  //  $form['operation'] = array(
  //    '#type' => 'select',
  //    '#options' => array('delete' => 'Delete'),
  //    '#title' => t('Operations'),
  //  );

  $form['operation'] = array(
    '#type' => 'hidden',
    '#value' => 'delete',
  );

  $form['mails'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('No content available.'),
  );

  $form['pager'] = array('#markup' => theme('pager', array('tags' => NULL)));

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete selected'),
  );

  return $form;
}

function mailing_list_emails_form_validate($form, &$form_state) {
  $operation = $form_state['values']['operation'];
  if (!$operation) {
    form_set_error('operation', 'No operation selected');
  }
}

function mailing_list_emails_form_submit(&$form, &$form_state) {
  if ($form_state['values']['operation'] == 'delete') {
    // This stupid hack is is from user.module.  How to get a confirm form to show
    // when you can't just pass the values via GET?  This is a problem.
    return;
  }

  // If other operations come into existance, they could happen here.
  $operation = $form_state['values']['operation'];
  $mails = $form_state['values']['mails'];
  $func = 'mailing_list_bulk_operation_' . $operation;
  if (!function_exists($func)) {
    // I'm not t()'ing this because no one will translate it anyway, and no one should see it.
    throw new Exception('Unable to process form, ' . $func . ' does not exist');
  }
  $mails = array_keys(array_filter($mails));
  call_user_func_array($func, array($mails));
}


function mailing_list_email_delete_multiple_confirm_form($form, &$form_state) {
  $edit = $form_state['input'];

  if ($mails = array_filter($edit['mails'])) {
    $result = db_select('mailing_list_emails', 'mle')
      ->fields('mle')
      ->condition('eid', $mails, 'IN')
      ->execute();

    $form['mails'] = array('#prefix' => '<ul>', '#suffix' => '</ul>', '#tree' => TRUE);
    foreach ($result as $row) {
      $form['mails'][$row->eid] = array(
        '#type' => 'hidden',
        '#value' => $row->eid,
        '#prefix' => '<li>',
        '#suffix' => check_plain($row->name . ' <' . $row->mail . '>') . "</li>\n",
      );
    }
  }
  else {
    drupal_set_message(t('Error, no mails were selected'), 'error');
    drupal_goto();
    return;
  }
  $form['operation'] = array('#type' => 'hidden', '#value' => 'delete');

  return confirm_form($form,
      t('Are you sure you want to delete these emails?'),
      'admin/people', t('This action cannot be undone.'),
      t('Delete emails from list'), t('Cancel'));
}

/**
 * Deletes multiple emails from admin listing screen.
 *
 * @param array $emails
 * @return void
 */
function mailing_list_email_delete_multiple_confirm_form_submit($form, &$form_state) {
  $mails = array_filter($form_state['values']['mails']);
  $result = db_delete('mailing_list_emails')
    ->condition('eid', $mails, 'IN')
    ->execute();

  drupal_set_message(t('Deleted %count emails from the database', array('%count' => count($mails))));
}

/**
 * Form for adding / renaming a mailing list.
 */
function mailing_list_form($form, &$form_state, $list = null) {
  if (empty($list)) {
    drupal_set_title(t('Add mailing list'));
  }
  else {
    drupal_set_title(t('Edit mailing list'));
  }

  $form = array();
  if (isset($form_state['values']['mlid']) || !empty($list)) {
    $form['mlid'] = array(
      '#type' => 'hidden',
      '#value' => isset($form_state['values']['mlid']) ? $form_state['values']['mlid'] : isset($list) ? $list->mlid: null,
    );
  }

  $form['name'] = array(
    '#title' => t('List name'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value'=> isset($form_state['values']['name']) ? $form_state['values']['name']: isset($list)?$list->name : '',
  );
  $form['submit'] = array(
    '#value' => t('Save'),
    '#type' => 'submit',
    '#submit' => array('mailing_list_form_submit'),
  );

  $form['#redirect'] = 'admin/structure/mailing-list';

  return $form;
}

/**
 * Submit handler for the add / rename mailing list form.
 */
function mailing_list_form_submit($form, &$form_state) {
  // @todo: put these in variables and on the settings page.
  $messages = array();

  $messages['update'] = array(
    'pass' => 'Renamed mailing list %name',
    'fail' => 'Failed to update mailing list %name',
  );

  $messages['insert'] = array(
    'pass' => 'Created mailing list %name',
    'fail' => 'Failed to create mailing list %name',
  );

  // List object to save
  $list = new StdClass();
  $list->name = $form_state['values']['name'];

  if (!empty($form_state['values']['mlid'])) {
    // Update operation
    $list->mlid = $form_state['values']['mlid'];
    $active_messages = $messages['update'];
  } else {
    $active_messages = $messages['insert'];
  }

  try {
    mailing_list_save($list);
    drupal_set_message(t($active_messages['pass'], array('%name' => $form_state['values']['name'])));
    watchdog('mailing_list', $active_messages['pass'], array('%name' => $form_state['values']['name']), WATCHDOG_NOTICE, l(t('view'), 'admin/structure/mailing-list/'. $list->mlid));
  }
  catch(PDOException $e) {
    drupal_set_message(t($active_messages['fail'], array('%name' => $form_state['values']['name'])), 'error');
    watchdog('mailing_list', $active_messages['fail'], array('%name' => $form_state['values']['name']), WATCHDOG_NOTICE);
  }
}

/**
 * Mailing list deletion form.
 *
 * @todo: D7 Upgrade this/
 */
function mailing_list_delete_confirm($form, $form_state, $list) {
  if (user_access('administer mailing lists')) {
    $form['mlid'] = array('#type' => 'value', '#value' => $list->mlid);
    $form['name'] = array('#type' => 'value', '#value' => $list->name);
    $output = confirm_form($form,
      t('Are you sure you want to delete mailing list %name? All e-mails in this list will be deleted too.', array('%name' => $list->name)),
      isset($_GET['destination']) ? $_GET['destination'] : 'admin/structure/mailing-list');
  }
  return $output;
}

/**
 * Submit handler for the mailing list deletion form.
 * @todo: D7 Upgrade this.
 */
function mailing_list_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    mailing_list_delete($form_state['values']['mlid']);
    // Remove corresponding block from all themes.
    module_invoke('block', 'flush_caches');
    drupal_set_message(t('Deleted mailing list %name.', array('%name' => $form_state['values']['name'])));
    watchdog('mailing list', 'Mailing list: deleted list %name.', array('%name' => $form_state['values']['name']));
    $form_state['redirect'] = 'admin/structure/mailing-list';
    return;
  }
}

/**
 * Form for importing a mailing list from a CSV file.
 */
function mailing_list_import_form($form, $form_state, $list) {
  if (empty($list)) {
    return;
  }

  drupal_set_title(t('Import e-mails'));

  $form = array();
  $form['#attributes'] = array('enctype' => "multipart/form-data");
  if ( isset($form_state['values']['mlid']) || isset($list) ) {
    $form['mlid']   = array(
      '#type'       => 'hidden',
      '#value'      => isset($form_state['values']['mlid']) ? $form_state['values']['mlid'] : isset($list) ? $list->mlid: null,
    );
  }
  if ( isset($form_state['values']['name']) || isset($list) ) {
    $form['name']   = array(
      '#type'       => 'hidden',
      '#value'      => isset($form_state['values']['name']) ? $form_state['values']['name'] : isset($list) ? $list->name: null,
    );
  }
  $form['file_upload'] = array(
    '#title'      => t('CSV file'),
    '#type'       => 'file',
  );
  $form['submit'] = array(
    '#value'      => t('Import'),
    '#type'       => 'submit',
  );
  return $form;
}

/**
 * Submit handler for the mailing list import form.
 *
 * @todo: D7, queries
 */
function mailing_list_import_form_submit($form, &$form_state) {
  $num_imported = 0;
  $mlid = $form_state['values']['mlid'];

  if ($file = file_save_upload('file_upload', array('file_validate_extensions' => array('csv')))) {
    ini_set('auto_detect_line_endings',TRUE);
    $handle = @fopen($file->uri, 'r');
    while (($data = fgetcsv($handle)) !== FALSE) {
      $num_fields = count($data);
      $is_existing = mailing_list_email_get_by_email($mlid, $data[0]);
      if (!$is_existing && $num_fields == 2) {
        $mle = new StdClass();
        $mle->mlid = $mlid;
        $mle->mail = $data[0];
        $mle->name = $data[1];
        mailing_list_email_save($mle);
        $num_imported++;
      }
    }

    fclose($handle);

    if ($num_imported) {
      drupal_set_message(t('Import complete: %num-imported added to mailing list %name.', array('%num-imported' => format_plural($num_imported, '1 e-mail', '@count e-mails'), '%name' => $form_state['values']['name'])));
    }
  }

  if (!$num_imported) {
    drupal_set_message(t('No e-mails added to mailing list: invalid or empty file.'), 'error');
  }

  $form_state['redirect'] = 'admin/structure/mailing-list/'. $form_state['values']['mlid'];

  return;
}

/**
 * Menu callback; exports a mailing list directly to the user as a CSV file.
 */
function mailing_list_export($list) {
	// create file to hold email list
	$filename = drupal_strtolower(str_replace(' ', '_', $list->name)). '.csv';
	$full_path = 'temporary' . '://' . $filename;

	$result = mailing_list_email_get_by_list($list->mlid, 0, 0);
	if (count($result) == 0) {
	  drupal_set_message('Cowardly refusing to export an empty mailing list');
	  drupal_goto('admin/structure/mailing-list/' . $list->mlid);
	}

	// create string with all the emails
	foreach($result as $row) {
		$emails[] = '"'. trim($row->mail) .'","'. trim($row->name) .'"';
	}

	$emails = implode("\n", $emails) ."\n";

	// write resultant string to file
  file_unmanaged_save_data($emails, $full_path, FILE_EXISTS_REPLACE);

	// set headers for file transfer
	$headers = array(
		'Content-Type' => 'application/octet-stream',
		'Content-Transfer-Encoding' => 'binary',
		'Content-Disposition' => 'attachment; filename="' . basename($full_path) . '";',
		'Content-Length' => sprintf('%u', filesize($full_path)),
	);

	file_transfer($full_path, $headers);
}

function mailing_list_admin_settings(&$form, &$form_state) {
  drupal_set_title('Mailing list settings');
  $form['mailing_list_subscription_message'] = array(
    '#type' => 'textfield',
    '#title' => t('Subscription message'),
    '#description' => t('The message users see when they subscribe.  You can use %mail and %name as tokens.'),
    '#default_value' => variable_get('mailing_list_subscription_message', MAILING_LIST_MESSAGE_SUBSCRIPTION),
  );

  return system_settings_form($form, TRUE);
}
