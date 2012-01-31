<?php

/**
 * @file
 * Hooks provided by the tac_alt_ui module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Respond to a vocabulary being enabled for taxonomy access control.
 *
 * @param $vid
 *   The vocabulary id being TAC-enabled.
 * @param $rid
 *   The role id for which this vocabulary is being enabled, or NULL if all
 *   roles are being enabled.
 */
function hook_taxonomy_access_enable_vocabulary($vid, $rid = NULL) {
  // Don't permit this vocabulary to be TAC-enabled for anonymous users.
  $vocabulary = taxonomy_vocabulary_load($vid);
  if ($vocabulary->machine_name == 'not_for_anon_access_control' && $rid == DRUPAL_ANONYMOUS_RID) {
    taxonomy_access_disable_vocab($vid, $rid);
  }
}

/**
 * @} End of "addtogroup hooks".
 */
