<?php

function tma_form_system_theme_settings_alter(&$form, $form_state)  {
  $form['breadcrumb'] = array(
    '#type' => 'fieldset',
    '#title' => t('Breadcrumb settings'),
  );

  $form['breadcrumb']['tma_breadcrumb'] = array(
    '#type' => 'select',
    '#title' => t('Display breadcrumb'),
    '#options' => array(
      'yes' => t('Yes'), 
      'admin' => t('Only in admin section'),
      'no' => t('No')
    ),
    '#default_value' => theme_get_setting('tma_breadcrumb'),
  );

  $form['breadcrumb']['tma_breadcrumb_separator'] = array(
    '#type' => 'textfield',
    '#title' => t('Breadcrumb separator'),
    '#description' => t('Text only. Don’t forget to include spaces.'),
    '#size' => 5,
    '#maxlength' => 10,
    '#default_value' => theme_get_setting('tma_breadcrumb_separator'),
  );

  $form['breadcrumb']['tma_breadcrumb_home'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show home page link in breadcrumb'),
    '#default_value' => theme_get_setting('tma_breadcrumb_home'),
  );
  $form['breadcrumb']['tma_breadcrumb_leading'] = array(
    '#type' => 'checkbox',
    '#title' => t('Prepend a separator to the beginning of the breadcrumb'),
    '#default_value' => theme_get_setting('tma_breadcrumb_leading'),
  );
  $form['breadcrumb']['tma_breadcrumb_trailing'] = array(
    '#type' => 'checkbox',
    '#title' => t('Append a separator to the end of the breadcrumb'),
    '#description' => t('Useful when the breadcrumb is placed just before the title.'),
    '#default_value' => theme_get_setting('tma_breadcrumb_trailing'),
  );
  $form['breadcrumb']['tma_breadcrumb_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Append the content title to the end of the breadcrumb'),
    '#description' => t('Useful when the breadcrumb is not placed just before the title.'),
    '#default_value' => theme_get_setting('tma_breadcrumb_title'),
  );
}
