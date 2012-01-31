<?php

/**
 * Implements hook_metatag_config_default().
 */
function metatag_metatag_config_default() {
  $configs = array();

  $config = new stdClass();
  $config->instance = 'global';
  $config->api_version = 1;
  $config->disabled = FALSE;
  $config->config = array(
    'title' => array('value' => '[current-page:title] | [site:name]'),
    'generator' => array('value' => 'Drupal 7 (http://drupal.org)'),
  );
  $configs[$config->instance] = $config;

  $config = new stdClass();
  $config->instance = 'global:frontpage';
  $config->api_version = 1;
  $config->disabled = FALSE;
  $config->config = array(
    'title' => array('value' => variable_get('site_slogan') ? '[site:name] | [site:slogan]' : '[site:name]'),
    'canonical' => array('value' => '[site:url]'),
  );
  $configs[$config->instance] = $config;

  $config = new stdClass();
  $config->instance = 'node';
  $config->api_version = 1;
  $config->disabled = FALSE;
  $config->config = array(
    'title' => array('value' => '[node:title] | [site:name]'),
    'description' => array('value' => '[node:summary]'),
  );
  $configs[$config->instance] = $config;

  if (module_exists('taxonomy')) {
    $config = new stdClass();
    $config->instance = 'taxonomy_term';
    $config->api_version = 1;
    $config->disabled = FALSE;
    $config->config = array(
      'title' => array('value' => '[term:name] | [site:name]'),
      'description' => array('value' => '[term:description]'),
    );
    $configs[$config->instance] = $config;
  }

  $config = new stdClass();
  $config->instance = 'user';
  $config->api_version = 1;
  $config->disabled = FALSE;
  $config->config = array(
    'title' => array('value' => '[user:name] | [site:name]'),
  );
  $configs[$config->instance] = $config;

  return $configs;
}

/**
 * Implements hook_metatag_config_instance_info().
 */
function metatag_metatag_config_instance_info() {
  $info['global']           = array('label' => t('Global'));
  $info['global:frontpage'] = array('label' => t('Front page'));
  // @todo The 403 and 404 meta tag contexts are disabled until they can be properly implemented.
  //$info['global:403']       = array('label' => t('403 page not found'));
  //$info['global:404']       = array('label' => t('404 page not found'));

  // Add instance information for entities.
  $entity_types = entity_get_info();
  foreach ($entity_types as $entity_type => $entity_info) {
    if (metatag_entity_supports_metatags($entity_type)) {
      $info[$entity_type] = array('label' => $entity_info['label']);
      foreach ($entity_info['bundles'] as $bundle => $bundle_info) {
        if (count($entity_info['bundles'] == 1) && $bundle == $entity_type) {
          // Skip default bundles (entities that do not really have bundles).
          continue;
        }
        if (metatag_entity_supports_metatags($entity_type, $bundle)) {
          $info[$entity_type . ':' . $bundle] = array('label' => $bundle_info['label']);
        }
      }
    }
  }

  return $info;
}

/**
 * Implements hook_metatag_info().
 */
function metatag_metatag_info() {
  $info['groups']['advanced'] = array(
    'label' => t('Advanced'),
    'form' => array(
      '#weight' => 110,
    ),
  );
  $info['tags']['description'] = array(
    'label' => t('Description'),
    'description' => t("A brief and concise summary of the page's content, preferably 150 characters or less. Google and other search engines display this information in search results."),
    'class' => 'DrupalTextMetaTag',
    'form' => array(
      '#type' => 'textfield',
      '#wysiwyg' => FALSE,
      '#size' => 120,
    ),
  );
  $info['tags']['keywords'] = array(
    'label' => t('Keywords'),
    'description' => t("A comma-separated list of words that describe the content. <em>Not</em> supported by most search engines."),
    'class' => 'DrupalTextMetaTag',
  );
  $info['tags']['title'] = array(
    'label' => t('Title'),
    'description' => t("The text displayed in a visitor's web browser title tab, as well as bookmarks and favorites."),
    'class' => 'DrupalTitleMetaTag',
  );

  // More advanced meta tags.
  $info['tags']['robots'] = array(
    'label' => t('Robots'),
    'description' => t("Provides search engines with specific directions for what to do when this page is indexed."),
    'class' => 'DrupalListMetaTag',
    'form' => array(
      '#options' => array(
        'noindex' => t('Prevent search engines from indexing this page.'),
        'nofollow' => t('Prevent search engines from following links on this page.'),
        'noarchive' => t('Prevents cached copies of this page from appearing in search results.'),
        'nosnippet' => t('Prevents descriptions from appearing in search results, and prevents page caching.'),
        'noodp' => t('Blocks the !opendirectory description from appearing in search results.', array('!opendirectory' => l('Open Directory Project', 'http://www.dmoz.org/'))),
      ),
    ),
    'group' => 'advanced',
  );
  $info['tags']['generator'] = array(
    'label' => t('Generator'),
    'description' => t("Describes the name and version number of the software or publishing tool used to create the page."),
    'class' => 'DrupalTextMetaTag',
    'element' => array(
      '#id' => 'system_meta_generator',
    ),
    'header' => 'X-Generator',
    'context' => array('global'),
    'group' => 'advanced',
  );
  $info['tags']['copyright'] = array(
    'label' => t('Copyright'),
    'description' => t("Details about intellectual property, such as copyright or trademarks. Does not automatically protect your site's content or intellectual property."),
    'class' => 'DrupalTextMetaTag',
    'group' => 'advanced',
  );

  // Link tags.
  $info['tags']['canonical'] = array(
    'label' => t('Canonical URL'),
    'description' => t("Preferred page location or URL to help eliminate duplicate content for search engines."),
    'class' => 'DrupalLinkMetaTag',
    'group' => 'advanced',
  );
  $info['tags']['shortlink'] = array(
    'label' => t('Shortlink URL'),
    'description' => t('A brief URL, often created by a URL shortening service.'),
    'class' => 'DrupalLinkMetaTag',
    'group' => 'advanced',
  );

  return $info;
}
