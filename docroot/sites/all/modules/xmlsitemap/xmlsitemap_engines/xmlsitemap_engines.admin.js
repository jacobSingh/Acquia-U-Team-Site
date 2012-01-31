// $Id$
(function ($) {

/**
 * Allow the custom URL textarea to be shown or hidden by the user.
 */
Drupal.behaviors.xmlsitemapAddMoreLink = {
  attach: function(context) {
    $('.form-item-xmlsitemap-engines-custom-urls', context).once('xmlsitemap-add-more-link', function () {
      // Define initial selectors associated with the custom URL textarea.
      var $textarea_wrapper = $(this);
      var $textarea_label = $('label[for="edit-xmlsitemap-engines-custom-urls"]', $textarea_wrapper);
      var $textarea = $('#edit-xmlsitemap-engines-custom-urls', $textarea_wrapper);

      // The link text for showing the textarea depends on whether or not the
      // textarea is already populated with URLs.
      if ($textarea.val()) {
        var add_more_link_text = Drupal.t('Show more');
      }
      else {
        var add_more_link_text = Drupal.t('Add more');
      }

      // Insert the links for showing or hiding the textarea.
      $textarea_wrapper.before('<a href="#" class="xmlsitemap-add-more-custom-urls">' + add_more_link_text + '</a>');
      $textarea_label.html(Drupal.t('!label (<a href="#" class="xmlsitemap-hide-custom-urls">hide</a>)', {'!label': $textarea_label.html()}));

      // Attach handlers to the links so that they show or hide the textarea,
      // as appropriate.
      $add_more_link = $('.xmlsitemap-add-more-custom-urls', context);
      $hide_link = $('.xmlsitemap-hide-custom-urls', context);
      $add_more_link.click(function () {
        $textarea_wrapper.show();
        $(this).hide();
        return false;
      });
      $hide_link.click(function () {
        $textarea_wrapper.hide();
        $add_more_link.show();
        return false;
      });

      // Start off by hiding the textarea when it is empty, and showing it
      // otherwise.
      if ($textarea.val()) {
        $add_more_link.trigger('click');
      }
      else {
        $hide_link.trigger('click');
      }
    });
  }
};

})(jQuery);
