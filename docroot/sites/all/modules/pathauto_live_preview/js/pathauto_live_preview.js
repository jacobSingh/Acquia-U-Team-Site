(function ($) {

Drupal.pathautoLivePreview = Drupal.pathautoLivePreview || {};

/**
 * Attaches behavior to the alias "edit" link to make it uncheck the "automatic alias" checkbox.
 */
Drupal.behaviors.pathautoEditLink = {
  attach: function (context, settings) {
    $('a#path-alias-edit-link', context).once('path-alias-edit-link', function() {
      $(this).click(function () {
        $('input[name="path[pathauto]"]').removeAttr('checked').change();
        return false;
      });
    });
  }
};

/**
 * Attaches behavior to the live alias preview to update itself when changes have been made to the form.
 *
 * Also copies the live alias preview to the manual alias input field when the
 * "automatic alias" checkbox becomes unchecked.
 */
Drupal.behaviors.pathautoLivePreview = {
  attach: function (context, settings) {
    // Setup the automatic alias preview to stay updated.
    $('input[name="path[pathauto_live_preview][ajax_trigger]"]').each(function () {
      if (this.id && settings.ajax && settings.ajax[this.id]) {
        var $ajax_trigger = $(this);
        var $alias_preview = $('#' + settings.ajax[this.id].wrapper, context);
        Drupal.pathautoLivePreview.init($alias_preview, $ajax_trigger);
      }
    });
    // Setup the manual alias input field to be initialized with the automatic
    // alias preview when the "automatic alias" checkbox becomes unchecked.
    $('input[name="path[pathauto]"]', context).once('pathauto-live-preview', function() {
      $(this).change(function () {
        if (!$(this).attr('checked')) {
          Drupal.pathautoLivePreview.edit($('.pathauto-live-preview', this.form).eq(0), $('input[name="path[alias]"]', this.form).eq(0));
        }
      });
    });
  }
};

/**
 * Makes the alias preview element "live" by binding a form change event handler.
 *
 * The form change event handler marks the preview element as needing an update,
 * and begins polling for when the preview element becomes visible. An AJAX
 * request is triggered only when the preview element needs an update *and* is
 * visible, so that AJAX requests aren't wasted updating an invisible element
 * (e.g., when its vertical tab isn't open).
 *
 * @todo Not all form fields impact the Pathauto alias. Instead of binding to a
 *   form change, this could be optimized to bind only to the form fields that
 *   affect the alias. This would require the server-side code to determine
 *   what those fields are based on the token pattern, and identify those fields
 *   within Drupal.settings.
 */
Drupal.pathautoLivePreview.init = function($alias_preview, $ajax_trigger) {
  $alias_preview.once('pathauto-live-preview', function() {
    var $form = $($ajax_trigger.get(0).form);
    $form.bind('change.pathautoLivePreview', function(event) {
      // Changing the text formatter in a wysiwyg editor fires a change event.
      // We do not want to run the pathautoLivePreview.poll function for change
      // events triggered by wysiwyg elements or we will break the wysiwyg.
      if(!$(event.target).hasClass('wysiwyg')) {
        $form.unbind('change.pathautoLivePreview');
        if (!$alias_preview.hasClass('pathauto-live-preview-needs-update')) {
          $alias_preview.addClass('pathauto-live-preview-needs-update');
          Drupal.pathautoLivePreview.poll($alias_preview, $ajax_trigger);
        }
      }
    });
  });
}

/**
 * Polls for whether the stale alias preview is visible, and when it is, triggers an AJAX request to update it.
 */
Drupal.pathautoLivePreview.poll = function($alias_preview, $ajax_trigger) {
  if ($alias_preview.is(':visible') && $ajax_trigger.is(':enabled')) {
    $ajax_trigger.change();
  }
  else {
    setTimeout(Drupal.pathautoLivePreview.poll.bind(null, $alias_preview, $ajax_trigger), 1000);
  }
}

/**
 * Copies the current alias preview into the manual alias input field.
 */
Drupal.pathautoLivePreview.edit = function($alias_preview, $alias_edit) {
  if ($alias_preview.length && $alias_edit.length && !$alias_preview.hasClass('pathauto-live-preview-no-alias')) {
    $alias_edit.val($alias_preview.text());
  }
}

})(jQuery);
