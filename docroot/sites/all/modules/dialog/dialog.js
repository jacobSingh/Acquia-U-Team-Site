(function ($) {

/**
 * Provides AJAX dialog updating via jQuery $.ajax (Asynchronous JavaScript and XML).
 */

Drupal.dialog = Drupal.dialog || {};

Drupal.behaviors.dialog = {
  attach: function (context, settings) {
    // Create the jQuery UI dialog box to handle all dialog events.
    $("body").once("dialog", function() {
      Drupal.dialog = $('<div id="dialog"></div>').dialog({
        autoOpen: false,
        modal: true
      });
    });

    // Display a loading modal for potential dialogs.
    $("a.use-dialog").once('use-dialog').click(function() {
      Drupal.ajax.prototype.commands.dialog_loading();
      return false;
    });
  }
};

/**
 * Provide the HTML to create the throbber.
 */
Drupal.theme.prototype.DialogThrobber = function () {
  var html = '';
  html += '  <div class="ajax-progress">';
  html += '    <div class="throbber"></div>';
  html += '  </div>';
  return html;
};

/**
 * Command to open a loading dialog.
 */
Drupal.ajax.prototype.commands.dialog_loading = function(ajax, response, status) {
  // @see http://drupal.org/node/1358624
  // For some reason, the ajax parameter is undefined. In the subsequent call to
  // dialog_display, ajax.settings is checked, so pass an empty object to
  // prevent an error.
  // @todo Refactor this to use the Drupal AJAX API's standard throbber instead
  // of injecting a custom one.
  Drupal.ajax.prototype.commands.dialog_display({}, {
    content: Drupal.theme('DialogThrobber'),
    title: Drupal.t('Loading...')
  });
};

/**
 * Command to display the dialog.
 */
Drupal.ajax.prototype.commands.dialog_display = function(ajax, response, status) {
  var element = Drupal.dialog;
  var new_content = $('<div></div>').html(response.content);
  element.dialog('close').html(new_content)
    // Restore the dialog's default values.
    .dialog('option', 'buttons', {})
    .dialog('option', 'width', '600px')
    .dialog('option', 'height', 'auto')
    .dialog('option', 'title', response.title);

  // Update the options available with the dialog.
  if (response.options) {
    jQuery.each(response.options, function(option, value) {
      element.dialog('option', option, value);
    });
  }

  // Apply any settings from the returned JSON if available.
  var settings = response.settings || ajax.settings || Drupal.settings;
  // Process any other behaviors on the content, and display the dialog box.
  Drupal.attachBehaviors(new_content, settings);
  element.dialog('open');
};

/**
 * Command to close the dialog.
 */
Drupal.ajax.prototype.commands.dialog_dismiss = function(ajax, response, status) {
  var element = Drupal.dialog;

  // Process any other behaviors on the content, and close the dialog box.
  Drupal.detachBehaviors(element);
  element.dialog('close');
};

/**
 * Command to redirect the browser.
 */
Drupal.ajax.prototype.commands.dialog_redirect = function(ajax, response, status) {
  window.location = response.url;
};

/**
 * Command to reload the browser.
 */
Drupal.ajax.prototype.commands.dialog_reload = function(ajax, response, status) {
  window.location.reload();
};

/**
 * Command to use the xLazyLoader to load additional JavaScript, CSS and images.
 *
 * http://code.google.com/p/ajaxsoft/wiki/xLazyLoader
 */
Drupal.ajax.prototype.commands.xlazyloader = function(ajax, response, status) {
  var settings = {
    name: 'lazy',
    success: function() {
	  // When it's complete loading the new JavaScript and CSS, make sure to run
	  // the behaviors on the object.
	  Drupal.attachBehaviors(Drupal.dialog);
    }
  };
  // Merge in the settings, allowing the loading of CSS and JavaScript.
  jQuery.extend(settings, response.options);
  // Load the scripts.
  $.xLazyLoader(settings);
};

})(jQuery);
