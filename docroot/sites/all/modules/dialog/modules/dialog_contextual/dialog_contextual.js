
(function ($) {

Drupal.behaviors.dialog_contextual = {
  attach: function (context, settings) {
    // Create the jQuery UI dialog box to handle all dialog events.
    $("ul.contextual-links a").once("dialog-contextual", function() {
      var destination = $(this).attr('href');
      if (destination.indexOf("?") == -1) {
        destination += "?dialog_contextual";
      }
      else {
        destination += "&dialog_contextual";
      }
      new Drupal.ajax($(this).attr('id'), this, {
        url: destination,
        event: "click",
        type: "GET" // TODO: File Drupal issue to allow changing the type of request.
      });
    }).addClass('use-dialog');
  }
};

})(jQuery);
