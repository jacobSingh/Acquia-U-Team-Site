<?php

/**
 * The plugin to handle Load More pager.
 *
 * @ingroup views_pager_plugins
 */
class views_plugin_pager_load_more extends views_plugin_pager_full {
  function summary_title() {
    if (!empty($this->options['offset'])) {
      return format_plural($this->options['items_per_page'], 'Show more pager, @count item, skip @skip', 'Show more pager, @count items, skip @skip', array('@count' => $this->options['items_per_page'], '@skip' => $this->options['offset']));
    }
      return format_plural($this->options['items_per_page'], 'Show more pager, @count item', 'Show more pager, @count items', array('@count' => $this->options['items_per_page']));
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['waypoint'] = array(
      'contains' => array(
        'infinite' => array('default' => FALSE),
      )
    );
    $options['more_button_text'] = array(
      'default' => t('Show more'),
      'translateable' => TRUE,
    );
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Provide a notice to users about enabling AJAX for use with this pager
    $form['load_more_notice'] = array(
      '#markup' => '<p>'. t('Important: To use \'Show more pager\', select \'Use AJAX: Yes\' in the Advanced section on the Edit view page') . '</p>',
      '#weight' => -20,
    );

    // It really doesn't make much sense for this pager to allow an unlimited number
    // of elements per page. Update description of the items to display field to reflect this
    $form['items_per_page']['#description'] = t('The number of items to display per page.');
    $form['items_per_page']['#weight'] = -10;

    // Provide an option for users to specify the text used on the more button.
    $form['more_button_text_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Rename \'Show more\' link'),
      '#weight' => -2,
    );
    $form['more_button_text'] = array(
      '#type' => 'textfield',
      '#title' => t('Rename \'Show more\' link'),
      '#title_display' => 'invisible',
      '#description' => t('The text that will be displayed on the link used to load more elements. For example "Show me more."'),
      '#default_value' => $this->options['more_button_text'] ? $this->options['more_button_text'] : t('Show more'),
      '#dependency' => array(
        'edit-pager-options-more-button-text-enable' => array(1),
      ),
      '#weight' => -1,
    );

    if (module_exists('waypoints')) {
      $form['waypoint'] = array(
        '#type' => 'fieldset',
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#tree' => TRUE,
        '#title' =>  t('Waypoint Support'),
        '#input' => TRUE,
        '#description' => t('Waypoints is a small jQuery plugin that makes it easy to execute a function whenever you scroll to an element.'),
      );
      $form['waypoint']['infinite'] = array(
        '#type' => 'checkbox',
        '#title' => t('Infinite scrolling'),
        '#description' => t('Show more content when the user reaches the bottom of the page.'),
        '#default_value' => $this->options['waypoint']['infinite'],
      );
    }
  }

  function options_validate(&$form, &$form_state) {
    parent::options_validate($form, $form_state);
    $items_per_page = $form_state['values']['pager_options']['items_per_page'];
    if($items_per_page == 0){
      form_set_error('options][items_per_page', t('When using the \'Show more pager\', the number of items per page must be at least 1'));
    }
  }

  function render($input) {
    if (module_exists('waypoints') && $this->options['waypoint']['infinite'] == 1) {
      $viewsLoadMore = array();
      $viewsLoadMore[] = array(
        'view_name' => $this->view->name,
        'view_display_id' => $this->view->current_display,
        'waypoints' => 'infinite',
      );
      drupal_add_library('waypoints', 'waypoints');
      drupal_add_js(array('viewsLoadMore' => $viewsLoadMore),'setting');
    }

    $pager_theme = views_theme_functions('views_load_more_pager', $this->view, $this->display);

    $vars = array(
      'parameters' => $input,
      'element' => $this->options['id'],
      'more_button_text' => $this->options['more_button_text'],
    );

    return theme($pager_theme, $vars);
  }
}
