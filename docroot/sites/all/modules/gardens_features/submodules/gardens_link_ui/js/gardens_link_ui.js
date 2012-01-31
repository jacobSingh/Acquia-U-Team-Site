/* 
 * Gardens Link UI
 *
 * Change aspects of the Link module UI in a lightweight way.
 */

(function ($) {
  $(document).ready(function ($) {
    // These are not meant to be permanent.
    $('[for$="title"]', '[class*="field-name-field"]').text(Drupal.t('Link text'));
    $('[for="edit-instance-settings-attributes-target-top"]').text(Drupal.t('Open link in same window'));
  });
}(jQuery));

