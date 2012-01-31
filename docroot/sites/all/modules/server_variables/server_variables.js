
Drupal.serverVariables = Drupal.serverVariables || {};

/**
 * Set initial variable field disable status based on checkbox value.
 * Bind click behaviour for checkboxes.
 */
Drupal.behaviors.serverVariables = {
  attach: function (context, settings) {
    var $ = jQuery;
    // get list of checkboxes
    var checkBoxes = $('input').filter(function () {
      return this.name.match(/^server_variables_.+?_ind$/); 
    }).once('server_variables');
    // set click handler
    checkBoxes.click(Drupal.serverVariables.setEnabled);
    // set initial value
    checkBoxes.each(Drupal.serverVariables.setEnabled);
  }
};

/**
 * Enable or disable variable name field based on checkbox checked / unchecked.
 */
Drupal.serverVariables.setEnabled = function() {
  var $ = jQuery;
  var varName = this.name.replace(/^(server_variables_.+?)_ind$/,'$1_var');
  var varField = $('input[name="' + varName + '"]');
  if (this.checked) {
    varField.attr('disabled', '');
  }
  else {
    varField.attr('disabled', 'disabled');
  }
}
