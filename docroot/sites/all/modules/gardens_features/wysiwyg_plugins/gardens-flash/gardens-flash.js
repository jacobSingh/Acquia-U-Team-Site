/*jslint bitwise: true, eqeqeq: true, immed: true, newcap: true, nomen: false,
 onevar: false, plusplus: false, regexp: true, undef: true, white: true, indent: 2
 browser: true */

/*global jQuery: true Drupal: true window: true ThemeBuilder: true */

/**
 * Fix the incorrect removal of '%' signs from width values of flash embeds.
 *
 * This plugin address an issue with CKEditor 3.5.1. This plugin can be disabled
 * and removed when the following issue is resolved:
 * @see http://dev.ckeditor.com/ticket/8586
 *
 * The related issue for iframes, http://dev.ckeditor.com/ticket/7114, was
 * resolved by the CKEditor dev team, so there is a good chance this one might
 * get addressed as well.
 */

(function ($) {
  Drupal.wysiwyg.plugins['gardens-flash'] = {
    /**
     * Prepare all plain-text contents of this plugin with HTML representations.
     *
     * @param content
     *   The plain-text contents of a textarea.
     * @param settings
     *   The plugin settings, as provided in the plugin's PHP include file.
     * @param instanceId
     *   The ID of the current editor instance.
     */
    attach: function (content, settings, instanceId) {
      return this.maintainPercentageUnit(content, settings, instanceId);
    },

    /**
     * Process all HTML placeholders of this plugin with plain-text contents.
     *
     * @param content
     *   The HTML content string of the editor.
     * @param settings
     *   The plugin settings, as provided in the plugin's PHP include file.
     * @param instanceId
     *   The ID of the current editor instance.
     */
    detach: function (content, settings, instanceId) {
      return this.maintainPercentageUnit(content, settings, instanceId);
    },
    /**
     * Fix the incorrect removal of '%' signs from width values of flash embeds.
     *
     * @param content
     *   The HTML content string of the editor.
     * @param settings
     *   The plugin settings, as provided in the plugin's PHP include file.
     * @param instanceId
     *   The ID of the current editor instance.
     */
    maintainPercentageUnit: function (content, settings, instanceId) {
      // jQueryify the content
      var $content = $('<div>', {
        html: content
      });
      $content.find('object, embed').each(function (index, element) {
        var $this = $(this);
        var stripPX = function (value) {
          var index = value.indexOf('px');
          if (index === -1) {
            return Number(value);
          }
          else {
            return Number(value.substring(0, index));
          }
        };
        var width = stripPX($this.attr('width')),
            height = stripPX($this.attr('height'));
        // If the width is 100, we might be dealing with an intended value of
        // 100% with the percent symbol stripped off.
        // @see http://dev.ckeditor.com/ticket/8586
        // If the height is greater than the width, we will assume that the
        // width of 100 is wrong, because this would result in a tall, skinny
        // video. This assumption has a small chance of being wrong. If so, this
        // plugin can be disabled for this wysiwyg editor on this text format.
        if (width === 100 && height > width) {
          $this.attr('width', '100%');
        }
      });

      // Return the original content as HTML.
      return $content.html();
    }
  };

}(jQuery));
