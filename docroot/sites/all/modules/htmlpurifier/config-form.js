// $Id: config-form.js,v 1.1.2.1 2010/10/27 17:13:44 ezyang Exp $
(function ($) {

Drupal.behaviors.htmlpurifierConfigForm = {
  // Makes all configuration links open in new windows; can save lots of grief!
  attach: function (context, settings) {
    $(".hp-config a", context).click(function () {
      window.open(this.href);
      return false;
    });
  }
};

})(jQuery);
