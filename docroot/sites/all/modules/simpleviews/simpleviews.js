// $Id: simpleviews.js,v 1.1.2.2 2008/10/18 14:59:19 eaton Exp $

jQuery(document).ready(function(){
  // @todo: no preview - this also breaks the layout in Seven.
  // jQuery("div#simpleviews-elements").prepend('<div id="simpleview-preview-img"></div>');
  jQuery("#edit-arg-toggle").bind("change", function() {
    if (this.checked) {
      $("#simpleviews-arg-wrapper").show();
    }
    else {
      $("#simpleviews-arg-wrapper").hide();
    }
    return false;
  })
});
