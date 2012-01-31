/*jslint bitwise: true, eqeqeq: true, immed: true, newcap: true, nomen: false,
 onevar: false, plusplus: false, regexp: true, undef: true, white: true, indent: 2
 browser: true */

/*global jQuery: true Drupal: true window: true ThemeBuilder: true */

(function ($) {

  Drupal.seoUI = Drupal.seoUI || {};

  Drupal.behaviors.seoUI = {
    attach: function (context, settings) {
      // Set the summary of the node edit form vertical tab
      $('fieldset.seo-settings', context).drupalSetSummary(function (context) {
        var output = [];
        // Path and Pathauto
        var path = $('.form-item-path-alias input').val();
        var automatic = $('.form-item-path-pathauto input').attr('checked');

        if (automatic) {
          output.push(Drupal.t('Automatic alias'));
        }
        else if (path) {
          output.push(Drupal.t('URL alias: @alias', {'@alias': path}));
        }
        else {
          output.push(Drupal.t('No alias'));
        }

        // Redirect
        if ($('table.redirect-list', context).size()) {
          if ($('table.redirect-list tbody td.empty', context).size()) {
            output.push(Drupal.t('No redirects'));
          }
          else {
            var redirects = $('table.redirect-list tbody tr').size();
            output.push(Drupal.formatPlural(redirects, '1 redirect', '@count redirects'));
          }
        }

        // Meta tags
        var vals = [];
        var metatags_enabled = false;
        $("input[name*='metatags'][type='text'], select[name*=metatags]", context).each(function () {
          // Assume reasonably that if we are in here Meta tags module is enabled.
          metatags_enabled = true;
          var default_name = $(this).attr('name').replace(/\[value\]/, '[default]');
          var default_value = $("input[type='hidden'][name='" + default_name + "']", context);
          if (default_value.length && default_value.val() === $(this).val()) {
            // Meta tag has a default value and form value matches default value.
            return true;
          }
          else if (!default_value.length && !$(this).val().length) {
            // Meta tag has no default value and form value is empty.
            return true;
          }
          // Show that something changed in any one of the metatags fields
          if (vals.length === 0) {
            vals.push(Drupal.t('Metatag defaults changed'));
          }
        });

        if (metatags_enabled) {
          if (vals.length === 0) {
            output.push(Drupal.t('Using Metatag defaults'));
          }
          else {
            output.push(vals);
          }
        }

        // HTML page title was moved out of Meta tags in its own fieldset.
        // Let the user know that the default value was changed
        var metatags_title = [];
        $("fieldset#edit-metatags-title input[type='text']", context).each(function () {
          var default_name = $(this).attr('name').replace(/value/, 'default');
          var default_value = $("input[type='hidden'][name='" + default_name + "']", context);
          if (default_value.length && default_value.val() === $(this).val()) {
            // Meta tag has a default value and form value matches default value.
            return true;
          }
          else if (!default_value.length && !$(this).val().length) {
            // Meta tag has no default value and form value is empty.
            return true;
          }
          else {
            metatags_title.push(Drupal.t('HTML page title: @value', {'@value': $(this).val()}));
          }
        });

        if (metatags_title.length !== 0) {
          output.push(metatags_title);
        }

        // Join the various vertical tab messages with a break tag in between them.
        return output.join('<br />');
      });
    }
  };
}(jQuery));
