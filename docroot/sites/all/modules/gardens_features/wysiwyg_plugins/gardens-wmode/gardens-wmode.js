/*jslint bitwise: true, eqeqeq: true, immed: true, newcap: true, nomen: false,
 onevar: false, plusplus: false, regexp: true, undef: true, white: true, indent: 2
 browser: true */

/*global jQuery: true Drupal: true window: true ThemeBuilder: true */

/**
 * When flash objects are not set to wmode=transparent, they do not participate
 * in the z-index layering of the other objects on the page.
 * Instead, they appear on top of every other object on the page.
 *
 * This CKEditor plugin configures flash obects to wmode=transparent
 * if wmode has not already been set on the object.
 */

(function ($) {

  /**
   * Determine if an element has an attribute and if that attribute is set
   * to value.
   */
  function hasAttribute (attribute, value) {
    // The attribute will be undefined if it does not exist on the element.
    var attr = this.attr(attribute);
    // If value is not provided to the function caller, it will equal the global
    // undefined object, not the 'undefined' string.
    if (value !== undefined) {
      // Check if the passed in value equals the value of the attribute.
      return (attr === value);
    }
    // Check whether the function has the attribute regardless of its value.
    return (attr !== undefined);
  }
  /**
   * Determine if an element has an attribute and if that attribute is set
   * to value.
   */
  function hasParam (param, value) {
    // The length of $param will be 0 if it does not exist on the element.
    var $param = this.children('param[name="' + param + '"]');
    var attr = $param.attr('value');
    if (value !== undefined) {
      // The value is defined and equals the passed-in value.
      return (attr === value);
    }
    // The attribute is defined, but it does not equal the passed-in value.
    return ($param.length > 0);
  }
  /**
   * Adds a parameter to an object.
   */
  function addParam (param, value) {
    // The length of $param will be 0 if it does not exist on the element.
    var $param = this.children('param[name="' + param + '"]');
    if ($param.length > 0) {
      $param.attr('value', value);
      return;
    }
    // Create the param because it doesn't exist yet.
    $('<param>', {
      name: param,
      value: value
    }).prependTo(this);
    return;
  }

  Drupal.wysiwyg.plugins = Drupal.wysiwyg.plugins || {};

  Drupal.wysiwyg.plugins['gardens-wmode'] = {
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
      return this.setWMode(content, settings, instanceId);
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
      return this.setWMode(content, settings, instanceId);
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
    setWMode: function (content, settings, instanceId) {
      // jQueryify the content
      var $content = $('<div>', {
        html: content
      });
      // Find object elements. We will assume that sites are using the twice-
      // cooked method of flash object embedding:
      // http://www.alistapart.com/articles/flashembedcagematch/
      // The twice-cooked method nests an embed element inside an object element
      // to address lack of uniform support for both the object and embed tags.
      $content.find('object').each(function (index, element) {
        var $object = $(this),
            // Get the embed inside this object if one exists.
            $embed = $object.find('embed'),
            hasEmbed = ($embed.length > 0),
            // Determine if the object has wmode configured.
            objectHasWMode = hasParam.apply($object, ['wmode']),
            isObjectTransparent = hasParam.apply($object, ['wmode', 'transparent']),
            // Determine if the embed exists and has wmode configured
            embedHasWMode = (hasEmbed && hasAttribute.apply($embed, ['wmode'])),
            isEmbedTransparent = (hasEmbed && hasAttribute.apply($embed, ['wmode', 'transparent']));
        // If wmode is not configured on the object, set it to transparent.
        if (!objectHasWMode) {
          addParam.apply($object, ['wmode', 'transparent']);
          objectHasWMode = isObjectTransparent = true;
        }
        // If wmode is not configured on the embed, set it to transparent.
        if (!embedHasWMode) {
          $embed.attr('wmode', 'transparent');
          embedHasWMode = isEmbedTransparent = true;
        }
        // If the object is transparent and the embed is not, make
        // the embed transparent.
        if (isObjectTransparent && (hasEmbed && !isEmbedTransparent)) {
          $embed.attr('wmode', 'transparent');
        }
        // If the embed is transparent and the object is not,
        // make the object transparent.
        if ((hasEmbed && isEmbedTransparent) && !isObjectTransparent) {
          addParam.apply($object, ['wmode', 'transparent']);
        }
      });

      // Return the original content as HTML.
      return $content.html();
    }
  };

}(jQuery));
