/*jslint bitwise: true, eqeqeq: true, immed: true, newcap: true, nomen: false,
 onevar: false, plusplus: false, regexp: true, undef: true, white: true, indent: 2
 browser: true */

/*global jQuery: true Drupal: true window: true ThemeBuilder: true */

(function ($) {

  Drupal.javascriptLibraries = Drupal.javascriptLibraries || {};

  Drupal.behaviors.javascriptLibraries = {
    attach: function (context) {
      // The drupalSetSummary method required for this behavior is not available
      // on the Blocks administration page, so we need to make sure this
      // behavior is processed only if drupalSetSummary is defined.
      if (typeof jQuery.fn.drupalSetSummary === 'undefined') {
        return;
      }
      $('fieldset#edit-javascript-libraries', context).drupalSetSummary(function (context) {
        var output = [],
            libraries = $('#edit-javascript-libraries-custom :checked', context);
        if (libraries.length > 0) {
          var limit = 2,
              extra = 0;
          libraries.each(function (index, element) {
            if (index < limit) {
              var $this = $(this);
              // Push the text of the checkbox label into the output.
              output.push($this.siblings('label').text().trim());
            }
            // If the limit is reached, just print 'and N more'.
            if (index >= limit) {
              var params = {
                '@extra': ++extra
              };
              output[limit] = Drupal.t(' and @extra more', params);
            }
          });
        }
        else {
          output.push(Drupal.t('No libraries selected'));
        }

        // Join the various vertical tab messages with a break in between them.
        return output.join('<br />');
      });
    }
  };
}(jQuery));
