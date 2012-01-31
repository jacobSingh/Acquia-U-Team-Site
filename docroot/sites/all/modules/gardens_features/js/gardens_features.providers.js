// $Id$
(function ($) {

/**
 * Allow the invitation message for new users to be shown or hidden.
 */
Drupal.behaviors.embedlyproviders = {
  attach: function(context, settings) {
    // Open a dialog with more information when a provider name is clicked.
    $('.provider-link', context).bind('click', this.dialogOpen);
    
    // Stop the overlay from opening an overlay within the media browser iframe.
    $('.embed-providers-link').bind('click', function (event) {
      event.stopPropagation();
    });
    
    $('.filter-links a', context).bind('click', this.filter);
  },
  dialogOpen: function (event) {
    event.preventDefault();
    $(event.target).siblings('.provider-info').clone().dialog({
      modal:true,
      title: $(event.target).clone(),
      draggable: false,
      resizable: false,
      width: 480,
      open: function () {
        // Close the dialog is the modal screen is clicked.
        $('.ui-widget-overlay').bind('click', Drupal.behaviors.embedlyproviders.dialogClose);
      }
    });
  },
  dialogClose: function (event) {
    $('.ui-dialog-content').dialog('destroy');
  },
  filter: function (event) {
    event.preventDefault();
    var $providers = $('.embedly-provider');
    $providers.show();

    var type = event.target.text.toLowerCase();
    
    if (type !== 'all') {
      $providers.not('.' + type).hide();
    } 
  }
};

}(jQuery));
