<?php

/**
 * @file
 * Migrate support for Redirect module.
 */

class MigrateRedirectEntityHandler extends MigrateDestinationHandler {
  public function __construct() {
    $this->registerTypes(array('entity'));
  }

  public function fields() {
    return array('migrate_redirects' => t('Original path(s) to redirect from.'));
  }


/**
 * Validate a redirect.
 * We need to check if a redirect already exists
 * otherwise if we call redirect_save in complete we get an db
 * error due to duplicate entries.
 *
 * This function is simmilar to the validate function in the
 * redirect module but the feedback is handled via the Migrate
 * saveMessage functionality.
 */
  private function redirectValidate($redirect) {
    $redirect = (object) $redirect;

    // check that there there are no redirect loops
    $migration = Migration::currentMigration();
    if (url($redirect->source) == url($redirect->redirect)) {
      $migration->saveMessage(t('Redirect to self (!redirect) ignored',
                              array('!redirect' => $redirect->redirect)),
                  MigrationBase::MESSAGE_INFORMATIONAL);
      return FALSE;
    }

    redirect_hash($redirect);
    if ($existing = redirect_load_by_hash($redirect->hash)) {
      if ($redirect->rid != $existing->rid) {
        $migration->saveMessage(t('The source path is already being redirected.'),
          MigrationBase::MESSAGE_INFORMATIONAL);
        return FALSE;
      }
    }

    return TRUE;
  }

  public function complete($entity, stdClass $row) {

    $migration = Migration::currentMigration();
    $destination = $migration->getDestination();
    $entity_type = $destination->getEntityType();

    // We looked up the destination entity_type in the constructor
    if (!empty($entity->migrate_redirects) && ($redirect_destination = entity_uri($entity_type, $entity))) {
      if (!is_array($entity->migrate_redirects)) {
        $entity->migrate_redirects = array($entity->migrate_redirects);
      }

      foreach ($entity->migrate_redirects as $path) {
        $redirect_defaults = array(
          'uid' => $entity->uid,
          'status_code' => 301,
          'language' => $entity->language,
        );
        $redirect = new stdClass();
        redirect_object_prepare($redirect, $redirect_defaults);
        $redirect->redirect = $redirect_destination['path'];
        $parsed = redirect_parse_url($path);
        $redirect->source = $parsed['path'];
        if (!empty($parsed['query'])) {
          $redirect->source_options['query'] = $parsed['query'];
        }

        // Only save if the redirect does not already exist.
        if ($this->redirectValidate($redirect)) {
          redirect_save($redirect);
        }
      }
    }
  }
}
