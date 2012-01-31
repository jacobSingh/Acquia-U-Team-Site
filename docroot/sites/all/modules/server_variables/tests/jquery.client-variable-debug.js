/*jslint bitwise: true, eqeqeq: true, immed: true, newcap: true, nomen: false,
 onevar: false, plusplus: false, regexp: true, undef: true, white: true, indent: 2
 browser: true */

/*global jQuery: true Drupal: true window: true */

/**
 * Debug plugin for the server_variables module
 */

(function ($) {
  Drupal = Drupal || {};
  Drupal.example = Drupal.example || {};
  Drupal.example.list = function (items) {
    var output = $('<ul>');
    for (var key in items) {
      if (items.hasOwnProperty(key)) {
        var $li = $('<li>');
        $li.append($('<span>', {
          text: key + ': '
        }));
         // If this is an object, recurse the function
        if (typeof(items[key]) === 'object') {
          $li.append(Drupal.example.list(items[key]));
        }
        else {
          $li.append($('<b>', {
            text: items[key]
          }));
        }
        $li.appendTo(output);
      }
    }
    return output;
  };
  $(document).ready(function ($) {
    if (Drupal && Drupal.settings && Drupal.settings.server_variables) {
      var variables = Drupal.settings.server_variables,
          list = Drupal.example.list(variables);
      if (list.children().length === 0) {
        list = $('<span>', {
          text: Drupal.t('No variable data available.')
        });
      }
      $('<div>', {
        id: 'server-variables-dialog',
        html: list
      })
      .appendTo($('body'))
      // Remove margins on uls
      .find('ul')
      .css({
        margin: 0
      })
      // back to the div
      .end()
      // Toss it in a dialog
      .dialog({
        title: Drupal.t('Server Variables'),
        minWidth: 400,
        position: ['right', 90],
        hide: 'slide'
      });
    }
  });
}(jQuery));