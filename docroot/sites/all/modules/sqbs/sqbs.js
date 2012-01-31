(function($) {

$(document).ready(function(){
  // Hide number of items in block initially if block is not to be exposed.
  if (!$('#edit-block').attr('checked')) {
    $(".form-item-block-items").hide();
  }
  
  // Attach change handler, so we maintain the option's visbility.
  $('#edit-block').bind("change", function() {
    if (this.checked) {
      $(".form-item-block-items").show();
      $('.form-item-block-items').yellowFade();
    }
    else {
      $(".form-item-block-items").hide();
    }
    return false;
  });
  
  // Change number of items based on style selection.
  $('#edit-style').bind("change", function() {
    var newDefault = 0;
    switch($('#edit-style').val()) {
      case 'full':
        newDefault = 5;
        break;
      case 'teasers':
        newDefault = 10;
        break;
      case 'titles':
      case 'table':
        newDefault = 25;
        break;
    }
    if (newDefault && ($('#edit-page-items').val() != newDefault)) {
      $('#edit-page-items').val(newDefault);
      $('.form-item-page-items').yellowFade();
    }
  });
});

$.fn.yellowFade = function() {
  return (this.css({backgroundColor: "#ffffcc"}).animate({
    backgroundColor: "#ffffff"
  }, 1500));
}
   
})(jQuery);
