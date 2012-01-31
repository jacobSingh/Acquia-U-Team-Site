// $Id: form_builder.js,v 1.23 2010/12/30 17:09:05 quicksketch Exp $

(function($) {

/**
 * @file form_builder.js
 * Provide enhancements to the form building user interface.
 */

Drupal.behaviors.formBuilderElement = {};
Drupal.behaviors.formBuilderElement.attach = function(context) {
  var $wrappers = $('div.form-builder-wrapper', context);
  var $elements = $('div.form-builder-element', context);

  // If the context itself is a wrapper, add it to the list.
  if ($(context).is('div.form-builder-wrapper')) {
    $wrappers = $wrappers.add(context);
  }

  // Add over effect on rollover.
  // The .hover() method is not used to avoid issues with nested hovers.
  $wrappers.not('div.form-builder-empty-placeholder')
    .bind('mouseover', Drupal.formBuilder.addHover)
    .bind('mouseout', Drupal.formBuilder.removeHover);

  // Add AJAX to edit links.
  $wrappers.find('span.form-builder-links a.configure').click(Drupal.formBuilder.editField);

  // Add AJAX to remove links.
  $wrappers.find('span.form-builder-links a.remove').click(Drupal.formBuilder.editField);

  // Add AJAX to entire field for easy editing.
  $elements.each(function() {
    if ($(this).children('fieldset.form-builder-fieldset').length == 0) {
      var link = $(this).parents('div.form-builder-wrapper:first').find('a.configure').get(0);
      if (link) {
        $(this).click(Drupal.formBuilder.clickField).addClass('form-builder-clickable');
        $(this).find('div.form-builder-element label').click(Drupal.formBuilder.clickField);
      }
      else {
        $(this).addClass('form-builder-draggable');
      }
    }
  });

  // Disable field functionality on click.
  $elements.find('input, textarea').bind('mousedown', Drupal.formBuilder.disableField);
};

/**
 * Behavior to disable preview fields and instead open up the configuration.
 */
Drupal.behaviors.formBuilderFields = {};
Drupal.behaviors.formBuilderFields.attach = function(context) {
  // Bind a function to all elements to update the preview on change.
  var $configureForm = $('#form-builder-field-configure', context);

  $configureForm.find('input, textarea, select')
    .not('.form-builder-field-change')
    .addClass('form-builder-field-change')
    .bind('change', Drupal.formBuilder.elementPendingChange);

  $configureForm.find('input.form-text, textarea')
    .not('.form-builder-field-keyup')
    .addClass('form-builder-field-keyup')
    .bind('keyup', Drupal.formBuilder.elementPendingChange);
};

/**
 * Behavior for the entire form builder. Add drag and drop to elements.
 */
Drupal.behaviors.formBuilder = {};
Drupal.behaviors.formBuilder.attach = function(context) {
  var formbuilder = $('#form-builder', context);

  $('.form-builder-wrapper:not(.ui-draggable)', formbuilder).draggable({
    opacity: 0.8,
    helper: 'clone',
    scroll: true,
    scrollSensitivity: 50,
    containment: 'body',
    start: Drupal.formBuilder.startDrag,
    stop: Drupal.formBuilder.stopDrag,
    change: Drupal.formBuilder.checkFieldsets,
    distance: 4,
    scope: 'fields',
    addClasses: false,
    appendTo: '#form-builder-wrapper'
  });

  // This sets the height of the drag target to be at least as hight as the field
  // palette so that field can be more easily dropped into an empty form.  IE6
  // does not respect min-height but does treat height in the same manner that
  // min-height would be expected.  So a check for browser and version is needed
  // here.
  var property = $.browser.msie && $.browser.version < 7 ? 'height' : 'min-height';
  formbuilder.css(property, $('#form-builder-fields').height());
};

/**
 * Behavior that renders fieldsets as tabs within the field configuration form.
 */
Drupal.behaviors.formBuilderTabs = {};
Drupal.behaviors.formBuilderTabs.attach = function(context) {
  var $fieldsets = $('fieldset.form-builder-group:not(.form-builer-tabs-processed)', context);
  var $close = $('<a class="close" href="#">' + Drupal.t('Close') + '</a>');
  var $tabs;
  var tabs = '';

  // Convert fieldsets to tabs.
  tabs = '<ul class="form-builder-tabs tabs clearfix">';
  $fieldsets.children('legend').each(function() {
    tabs += '<li>' + this.innerHTML + '</li>';
    $(this).remove();
  });
  tabs += '</ul>';

  // Add the new tabs to the page.
  $tabs = $(tabs);
  $fieldsets.filter(':first').before($close).before($tabs);

  // Hide all the fieldsets except the first.
  $fieldsets.not(':first').css('display', 'none');
  $tabs.find('li:first').addClass('active').click(Drupal.formBuilder.clickCancel);

  // Enable tab switching by clicking on each tab.
  $tabs.find('li:not(.close)').each(function(index) {
    $(this).click(function() {
      $fieldsets.filter(':visible').css('display', 'none');
      $fieldsets.eq(index).css('display', 'block');
      $tabs.find('li.active').removeClass('active').unbind('click', Drupal.formBuilder.clickCancel);
      $(this).addClass('active').click(Drupal.formBuilder.clickCancel);
      Drupal.formBuilder.fixTableDragTabs($fieldsets.eq(index).get(0));
    });
  });

  $close.click(Drupal.formBuilder.clickCancel);

  // Add guard class.
  $fieldsets.addClass('form-builer-tabs-processed');
};

/**
 * Submit the delete form via AJAX or close the form with the cancel link.
 */
Drupal.behaviors.formBuilderDeleteConfirmation = {};
Drupal.behaviors.formBuilderDeleteConfirmation.attach = function(context) {
  var $confirmForm = $('form.confirmation', context);
  if ($confirmForm.length) {
    $confirmForm.find('input[type=submit]').bind('click', function(event) {
      event.preventDefault();
      // Store the form and the options
      var form = $confirmForm;
      Drupal.formBuilder.ajaxOptions = {
        url: form.attr('action'),
        success: Drupal.formBuilder.deleteField,
        error: Drupal.formBuilder.ajaxError,
        type: 'post',
        dataType: 'json',
        action: 'deleteConfirmation',
        data: form.serialize(),
        tryCount: 0,
        maxTry: 3
      };
      // Submit the form via ajax
      $.ajax(Drupal.formBuilder.ajaxOptions);
      // Bind this action to disable any submit buttons on the page.  It will be
      // removed on success or after the retries have been exhausted.
      $('form').submit(Drupal.formBuilder.preventSubmit);
    });
    $confirmForm.find('a').click(Drupal.formBuilder.clickCancel);
  }
};

/**
 * Keeps record of if a mouse button is pressed.
 */
Drupal.behaviors.formBuilderMousePress = {};
Drupal.behaviors.formBuilderMousePress.attach = function(context) {
  if (context == document) {
    $('body').mousedown(function() { Drupal.formBuilder.mousePressed = 1; });
    $('body').mouseup(function() { Drupal.formBuilder.mousePressed = 0; });
  }
};

/**
 * Scrolls the add new field block with the window.
 */
Drupal.behaviors.formBuilderBlockScroll = {};
Drupal.behaviors.formBuilderBlockScroll.attach = function(context) {
  var $list = $('ul.form-builder-fields', context);

  if ($list.length && $list.hasClass('block-scroll')) {
    var $block = $list.parents('div.block:first').css('position', 'relative');
    var blockScrollStart = $block.offset().top;

    function blockScroll() {
      // Do not move the palette while dragging a field.
      if (Drupal.formBuilder.activeDragUi) {
        return;
      }

      var windowOffset = $(window).scrollTop();
      var blockHeight = $block.height();
      var formBuilderHeight = $('#form-builder').height();
      if (windowOffset - blockScrollStart > 0) {
        // Do not scroll beyond the bottom of the editing area.
        var newTop = Math.min(windowOffset - blockScrollStart + 20, formBuilderHeight - blockHeight);
        $block.animate({ top: (newTop + 'px') }, 'fast');
      }
      else {
        $block.animate({ top: '0px' }, 'fast');
      }
    }

    var timeout = false;
    function scrollTimeout() {
      if (timeout) {
        clearTimeout(timeout);
      }
      timeout = setTimeout(blockScroll, 100);
    }

    $(window).scroll(scrollTimeout);
  }
};

/**
 * Behavior for the Add a field block.
 * @param {Object} context
 */
Drupal.behaviors.formBuilderNewField = {};
Drupal.behaviors.formBuilderNewField.attach = function(context) {
  var $list = $('ul.form-builder-fields', context);

  if ($list.length) {
    // Allow items to be copied from the list of new fields.
    $list.children('li:not(.ui-draggable)').draggable({
      opacity: 0.8,
      helper: 'clone',
      scroll: true,
      scrollSensitivity: 50,
      containment: 'body',
      start: Drupal.formBuilder.startDrag,
      stop: Drupal.formBuilder.stopDrag,
      distance: 4,
      scope: 'fields',
      appendTo: '#form-builder-wrapper'
    })
    .bind('click', function(event) {
      event.preventDefault();
      event.stopPropagation();
      var item = $(this).clone();
      // Add a drop target at the bottom of the form to automatically drop the
      // element onto.
      var placeholder = $('<div class="form-builder-placeholder"></div>');
      placeholder
        .appendTo('#form-builder')
        .droppable({
          scope: 'fields',
          drop: Drupal.formBuilder.drop
        });
      Drupal.formBuilder.drop({data: placeholder}, {draggable: item});
      // Pass the element to the stopDrag method so that unique elements are
      // properly hidden in the palette.
      Drupal.formBuilder.stopDrag.apply(this);
    });
  }
};

Drupal.formBuilder = {
  // Variable to prevent multiple requests.
  updatingElement: false,
  // Variables to allow delayed updates on textfields and textareas.
  updateDelayElement: false,
  updateDelay: false,
  // Variable holding the actively edited element (if any).
  activeElement: false,
  // Variable holding the active drag object (if any).
  activeDragUi: false,
  // Variable of the time of the last update, used to prevent old data from
  // replacing newer updates.
  lastUpdateTime: 0,
  // Status of mouse click.
  mousePressed: 0,
  // Field configure form target
  fieldConfigureHolder: false
};

/**
 * Event callback for mouseover of fields. Adds hover class.
 */
Drupal.formBuilder.addHover = function() {
  // Do not add hover effect while dragging over other fields.
  if (!Drupal.formBuilder.activeDragUi && !Drupal.formBuilder.mousePressed) {
    if ($(this).find('div.form-builder-hover').length == 0) {
      $(this).addClass('form-builder-hover');
    }
  }
};

/**
 * Event callback for mouseout of fields. Removes hover class.
 */
Drupal.formBuilder.removeHover = function() {
  // Do not add hover effect while dragging over other fields.
  if (!Drupal.formBuilder.activeDragUi && !Drupal.formBuilder.mousePressed) {
    $(this).removeClass('form-builder-hover');
  }
};

/**
 * Click handler for fields.
 *
 * Note this is applied to both the entire field and to the labels within the
 * field, as they have special browser behavior that needs to be overridden.
 */
Drupal.formBuilder.clickField = function(e) {
  // Allow select lists to be clicked on without opening the edit options.
  if ($(e.target).is('select')) {
    return;
  }

  var wrapper = $(this).parents('.form-builder-wrapper:first');
  // This will get the first configure link that does not belong to a nested form element
  // inside this form element.
  var link = wrapper.find('a.configure').not(wrapper.find('.form-builder-element .form-builder-element a')).get(0);
  
  Drupal.formBuilder.editField.apply(link);

  return false;
};

/**
 * Mousedown event on element previews.
 */
Drupal.formBuilder.disableField = function(e) {
  return false;
};

/**
 * Load the edit form from the server.
 */
Drupal.formBuilder.editField = function(event) {
  if (event && $(event.target).is('a')) {
    event.stopPropagation();
  }
  var element = $(this).parents('div.form-builder-wrapper').get(0);
  var link = $(this);

  // Prevent duplicate clicks from taking effect if already handling a click.
  if (Drupal.formBuilder.updatingElement) {
    return false;
  }

  link.addClass('progress');

  // If clicking on the link a second time, close the form instead of open.
  if (element == Drupal.formBuilder.activeElement && link.get(0) == Drupal.formBuilder.activeLink) {
    Drupal.formBuilder.closeActive(function() {
      link.removeClass('progress');
      if (Drupal.formBuilder.fieldConfigureForm) {
        Drupal.formBuilder.fieldConfigureForm.html($('<div class="field-settings-message">' + Drupal.t('No field selected') + '</div>'));
      }
    });
    Drupal.formBuilder.unsetActive();
    return false;
  }

  if (!Drupal.formBuilder.fieldConfigureForm) {
    $('<div id="#form-builder-field-configure" class="form-builder-field-configure"><div class="field-settings-message">' + Drupal.t('Loading...') + '</div></div>').appendTo(element);
  } else {
    $('.field-settings-message').remove();
    Drupal.formBuilder.fieldConfigureForm.append($('<div class="field-settings-message">' + Drupal.t('Loading...') + '</div>'));
  }
  

  var getForm = function() {
    Drupal.formBuilder.ajaxOptions = {
      url: link.attr('href'),
      type: 'GET',
      dataType: 'json',
      data: 'js=1',
      success: Drupal.formBuilder.displayForm,
      error: Drupal.formBuilder.ajaxError,
      errorMessage: Drupal.t('Form could not be loaded at this time. Please try again later.'),
      tryCount: 0,
      maxTry: 3
    }
    
    $.ajax(Drupal.formBuilder.ajaxOptions);
  };

  Drupal.formBuilder.updatingElement = true;
  Drupal.formBuilder.closeActive(getForm);
  Drupal.formBuilder.setActive(element, link.get(0));

  return false;
};

/**
 * Click handler for deleting a field.
 */
Drupal.formBuilder.deleteField = function(callback) {
  var active = $(Drupal.formBuilder.activeElement);
  // Renable form submission.
  $('form').unbind('submit', Drupal.formBuilder.preventSubmit);
  active.fadeOut(function() {
    // If this is a unique field, show the field in the palette again.
    var elementId = active.find('.form-builder-element').attr('id');
    $('ul.form-builder-fields').find('li.' + elementId).show('slow');
    // Remove the field from the form.
    active.remove();
    // Close the form and unset the active element
    Drupal.formBuilder.clickCancel();
    // Check for empty fieldsets.
    Drupal.formBuilder.checkFieldsets(null, null, true);

    if (callback && $.isFunction(callback)) {
      callback();
    }
  });
};

Drupal.formBuilder.clickCancel = function() {
  Drupal.formBuilder.closeActive();
  Drupal.formBuilder.unsetActive();
  return false;
};

/**
 * Display the edit form from the server.
 */
Drupal.formBuilder.displayForm = function(response) {
  if (response.settings) {
    $.extend(true, Drupal.settings, response.settings);
  }
  var $preview = $('#form-builder-element-' + response.elementId);
  var $form = $(response.html)
  if (!Drupal.formBuilder.fieldConfigureForm) {
    $('.form-builder-field-configure').html($form);
    $form.css('display', 'none');
  } else {
    Drupal.formBuilder.fieldConfigureForm.html($form);
    $form.css('visibility: hidden');
  }
  $('.field-settings-message').remove();
  Drupal.attachBehaviors($form.parent().get(0));

  $form
    // Add the ajaxForm behavior to the new form.
    .ajaxForm()
    // Using the 'data' $.ajaxForm property doesn't seem to work.
    // Manually add a hidden element to pass additional data on submit.
    .prepend('<input type="hidden" name="return" value="field" />')
    // Add in any messages from the server.
    .find('fieldset:visible:first').prepend(response.messages);

  $form.css({visibility: 'visible', display: 'none'});
  $form.slideDown(function() {
    $preview.parents('.form-builder-wrapper:first').find('a.progress').removeClass('progress');
  });

  // Give focus to the form
  $form.find('input:visible:first').focus();

  Drupal.formBuilder.updatingElement = false;
};

/**
 * Upon changing a field, submit via AJAX to the server.
 */
Drupal.formBuilder.elementChange = function() {
  if (!Drupal.formBuilder.updatingElement) {
    // Store the form and the options
    var form = $(this).parents('form:first');
    Drupal.formBuilder.ajaxOptions = {
      url: form.attr('action'),
      success: Drupal.formBuilder.updateElement,
      error: Drupal.formBuilder.ajaxError,
      type: 'post',
      dataType: 'json',
      errorMessage: Drupal.t('Field could not be updated at this time. Please try again later.'),
      data: form.serialize(),
      tryCount: 0,
      maxTry: 3
    };
    // Submit the form via ajax
    $.ajax(Drupal.formBuilder.ajaxOptions);

    // Bind this action to disable any submit buttons on the page.  It will be
    // removed on success or after the retries have been exhausted.
    $('form').submit(Drupal.formBuilder.preventSubmit);
  }

  // Clear any pending updates until further changes are made.
  if (Drupal.formBuilder.updateDelay) {
    clearTimeout(Drupal.formBuilder.updateDelay);
  }

  Drupal.formBuilder.updatingElement = true;
};

Drupal.formBuilder.ajaxError = function (XMLHttpRequest, textStatus, errorThrown) {
  var options = Drupal.formBuilder.ajaxOptions;
  var message = this.errorMessage ? this.errorMessage : 'Unable to reach server.  Please try again later.';

  options.tryCount++;
  if (options.tryCount <= options.maxTry) {
    $.ajax(options);
  } else {
    $('form').unbind('submit', Drupal.formBuilder.preventSubmit);
    alert(message);
  }
};

Drupal.formBuilder.preventSubmit = function (event) {
  event.preventDefault();
}

/**
 * Update a field after a delay.
 *
 * Similar to immediately changing a field, this field as pending changes that
 * will be updated after a delay. This includes textareas and textfields in
 * which updating continuously would be a strain the server and actually slow
 * down responsiveness.
 */
Drupal.formBuilder.elementPendingChange = function(e) {
  // Only operate on "normal" keys, excluding special function keys.
  // http://protocolsofmatrix.blogspot.com/2007/09/javascript-keycode-reference-table-for.html
  if (e.type == 'keyup' && !(
    e.keyCode >= 48 && e.keyCode <= 90 || // 0-9, A-Z.
    e.keyCode >= 93 && e.keyCode <= 111 || // Number pad.
    e.keyCode >= 186 && e.keyCode <= 222 || // Symbols.
    e.keyCode == 8) // Backspace.
    ) {
    return;
  }

  if (Drupal.formBuilder.updateDelay) {
    clearTimeout(Drupal.formBuilder.updateDelay);
  }
  Drupal.formBuilder.updateDelayElement = this;
  Drupal.formBuilder.updateDelay = setTimeout("Drupal.formBuilder.elementChange.apply(Drupal.formBuilder.updateDelayElement, [true])", 500);
};

/**
 * After submitting the change to the server, display the updated element.
 */
Drupal.formBuilder.updateElement = function(response) {
  var $configureForm = $('.form-builder-field-configure');

  // Do not let older requests replace newer updates.
  if (response.time < Drupal.formBuilder.lastUpdateTime) {
    return;
  }
  else {
    Drupal.formBuilder.lastUpdateTime = response.time;
  }

  // Update Drupal.settings.
  if (response.settings) {
    $.extend(true, Drupal.settings, response.settings);
  }

  // Set the error class on fields.
  $configureForm.find('.error').removeClass('error');
  if (response.errors) {
    for (var elementName in response.errors) {
      elementName = elementName.replace(/([a-z0-9_]+)\](.*)/, '$1$2]');
      $configureForm.find('[name=' + elementName + ']').addClass('error');
    }
  }

  // Display messages, if any.
  $configureForm.find('.messages').remove();
  if (response.messages) {
    $configureForm.find('fieldset:visible:first').prepend(response.messages);
  }

  // Do not update the element if errors were received.
  if (!response.errors) {
    var $exisiting = $('#form-builder-element-' + response.elementId);
    var $new = $(response.html).find('.form-builder-element:first');
    $exisiting.replaceWith($new);

    // Expand root level fieldsets after updating to prevent them from closing
    // after every update.
    $new.children('fieldset.collapsible').removeClass('collapsed');
    Drupal.attachBehaviors($new.parent().get(0));
  }

  // Set the variable stating we're done updating.
  Drupal.formBuilder.updatingElement = false;
  $('form').unbind('submit', Drupal.formBuilder.preventSubmit);
};

/**
 * When adding a new field, remove the placeholder and insert the new element.
 */
Drupal.formBuilder.addElement = function(response) {
  // This is very similar to the update element callback, only we replace the
  // entire wrapper instead of just the element.
  if (response.settings) {
    $.extend(true, Drupal.settings, response.settings);
  }
  var $exisiting = $('.form-builder-new-field');
  var $new = $(response.html);
  $exisiting.replaceWith($new);
  Drupal.attachBehaviors($new.get(0));

  $new.draggable('destroy');
  $new.draggable({
    opacity: 0.8,
    helper: 'clone',
    scroll: true,
    scrollSensitivity: 50,
    containment: 'body',
    start: Drupal.formBuilder.startDrag,
    stop: Drupal.formBuilder.stopDrag,
    change: Drupal.formBuilder.checkFieldsets,
    distance: 4,
    scope: 'fields',
    addClasses: false,
    appendTo: '#form-builder-wrapper'
  });

  // Set the variable stating we're done updating.
  Drupal.formBuilder.updatingElement = false;

  // Insert the new position form containing the new element.
  $('#form-builder-positions').replaceWith(response.positionForm);

  // Submit the new positions form to save the new element position.
  Drupal.formBuilder.updateElementPosition($new);
};

/**
 * Given an element, update it's position (weight and parent) on the server.
 */
Drupal.formBuilder.updateElementPosition = function(element) {
  // Update weights of all children within this element's parent.
  element.parent().children('.form-builder-wrapper').each(function(index) {
    var child_id = $(this).children('.form-builder-element:first').attr('id');
    $('#form-builder-positions input.form-builder-weight').filter('.' + child_id).val(index);
  });

  // Update this element's parent.
  var $parent = element.parents('.form-builder-element:first');
  var parent_id = $parent.length ? $parent.attr('id').replace(/form-builder-element-(.*)/, '$1') : 0;
  var child_id = element.children('.form-builder-element:first').attr('id');
  $('#form-builder-positions input.form-builder-parent').filter('.' + child_id).val(parent_id);

  // Submit the position form via AJAX to save the new weights and parents.
  $('#form-builder-positions').ajaxSubmit();
};

/**
 * Called when a field has been moved via Sortables.
 *
 * @param e
 *   The event object containing status information about the event.
 * @param ui
 *   The jQuery Sortables object containing information about the sortable.
 */
Drupal.formBuilder.drop = function(e, ui) {
  var element = ui.draggable;
  var placeholder = e.data ? $(e.data) : $(this);
  
  // If no drop target is hit, add the component to the end of the form.
  if (placeholder.is('#form-builder')) {
    placeholder = placeholder.children(':last');
  } else if (placeholder.is('.form-builder-wrapper:not(.form-builder-empty-placeholder)')) {
    placeholder = placeholder.next('.form-builder-placeholder');
  }

  // If the element is a new field from the palette, update it with a real field.
  if (element.is('.ui-draggable')) {
    var name = 'new_' + new Date().getTime();
    // If this is a "unique" element, its element ID is hard-coded.
    if (element.is('.form-builder-unique')) {
      name = element.className.replace(/^.*?form-builder-element-([a-z0-9_]+).*?$/, '$1');
    }

    var $ajaxPlaceholder = $('<div class="form-builder-wrapper form-builder-new-field"><div id="form-builder-element-' + name + '" class="form-builder-element"><span class="progress">' + Drupal.t('Please wait...') + '</span></div></div>');
    placeholder.replaceWith($ajaxPlaceholder);

    Drupal.formBuilder.ajaxOptions = {
      url: element.find('a').attr('href'),
      type: 'GET',
      dataType: 'json',
      data: 'js=1&element_id=' + name,
      success: Drupal.formBuilder.addElement,
      error: Drupal.formBuilder.ajaxError,
      errorMessage: Drupal.t('Element could not be added at this time. Please try again later.'),
      tryCount: 0,
      maxTry: 3
    };
    $.ajax(Drupal.formBuilder.ajaxOptions);

    Drupal.formBuilder.updatingElement = true;
  }
  // Update the positions (weights and parents) in the form cache.
  else {
    placeholder.replaceWith(element);
    element.removeClass('original').show();
    ui.helper.remove();
    Drupal.formBuilder.updateElementPosition(element);

    // Select the element
    element.find('a.configure').click();
  }

  Drupal.formBuilder.activeDragUi = false;
  $('#form-builder .form-builder-placeholder').remove();
  
  // Update empty fieldsets
  Drupal.formBuilder.checkFieldsets();

  // Scroll the palette into view.
  $(window).scroll();
};

/**
 * Called when a field is about to be moved from the new field palette.
 *
 * @param e
 *   The event object containing status information about the event.
 * @param ui
 *   The jQuery Sortables object containing information about the sortable.
 */
Drupal.formBuilder.startDrag = function(e, ui) {
  var $this = $(this);
  // Check to see if this has been pulled out of a fieldset.
  Drupal.formBuilder.checkFieldsets();

  ui.helper.width($(this).width());

  if ($this.is('.form-builder-unique')) {
    $this.css('visibility', 'hidden');
  }
  if ($this.is('.form-builder-wrapper')) {
    $this.hide();
  }
  $this.addClass('original');

  var isPagebreak = false;
  if ($this.children('.form-builder-element-pagebreak').length > 0) {
    isPagebreak = true;
  }
  if ($this.is('.field-pagebreak')) {
    isPagebreak = true;
  }

  // Grab the formbuilder and fields and store the placeholder markup.
  var formbuilder = $('#form-builder');
  var fields = $('#form-builder').find('.form-builder-wrapper')
    .not(this)
    .not('.form-builder-empty-placeholder')
    .not($('.original *'));
  var placeholder = '<div class="form-builder-placeholder"></div>';

  // Add a drop target at the bottom of the form.
  $(placeholder).appendTo(formbuilder);

  if (fields.length) {
    // Insert a drop target after each field.
    $(placeholder).insertBefore(fields);
    // If we're not dragging a page break, add a drop target at the bottom of each fieldset.
    if (!isPagebreak) {
      $(placeholder).appendTo('.fieldset-wrapper:not(:has(.form-builder-empty-placeholder))');
    }
  } else {
    // The form is empty, so make the placeholder at least the size of the form.
    formbuilder.find('.form-builder-placeholder').css('min-height', formbuilder.css('height'));
  }

  var height = formbuilder.height();
  formbuilder.children(':visible:not(.form-builder-placeholder)').each(function() {
    height = height - $(this).outerHeight(true);
  });
  if (height > 0) {
    $('.form-builder-placeholder:last').css('min-height', height);
  }

  // Activate all the placeholders as drop targets.
  var placeholders = $('.form-builder-placeholder, .form-builder-empty-placeholder');
  if (isPagebreak) {
    placeholders = placeholders.not('.fieldset-wrapper .form-builder-placeholder').not('.fieldset-wrapper .form-builder-empty-placeholder');
  }
  
  placeholders.droppable({
    scope: 'fields',
    tolerance: 'touch',
    over: Drupal.formBuilder.over,
    out: Drupal.formBuilder.out,
    drop: Drupal.formBuilder.drop
  });

  // Add droppable to form elements without the over / out methods
  $('.form-builder-wrapper').droppable({
    scope: 'fields',
    tolerance: 'touch',
    drop: Drupal.formBuilder.drop
  });

  // Retain the dimensions of the draggable
  ui.helper.width($this.width());
  ui.helper.height($this.height());

  Drupal.formBuilder.activeDragUi = ui;
};

/**
 * Called after a field has been moved from the new field palette.
 *
 * @param e
 *   The event object containing status information about the event.
 * @param ui
 *   The jQuery Sortables object containing information about the sortable.
 */
Drupal.formBuilder.stopDrag = function(e, ui) {
  var $this = $(this);
  // Remove the droppable from all elements within the form
  $('#form-builder .ui-droppable').droppable('destroy');

  // If the activeDragUi is still set, we did not drop onto the form.
  if (Drupal.formBuilder.activeDragUi) {
    ui.helper.remove();
    Drupal.formBuilder.activeDragUi = false;
    $this.css('visibility', '').show().removeClass('original');
    $(window).scroll();
    // Remove the placeholders
    $('#form-builder .form-builder-placeholder').remove();
    
    Drupal.formBuilder.checkFieldsets();
  }
  // If dropped onto the form and a unique field, remove it from the palette.
  else if ($this.is('.form-builder-unique')) {
    $this.animate({height: '0', width: '0'}, function() {
      $this.css({visibility: '', height: '', width: '', display: 'none'});
    });
  }
};

/**
 * These functions add and remove a class to a drop target and adjust its height
 * to the height of the item being dragged.
 */
Drupal.formBuilder.over = function(e, ui) {
  var $this = $(this);
  if (!$this.is('.form-builder-empty-placeholder')) {
    $this.height(ui.draggable.height());
  }
  $this.parent().find('.over').height(0);
  $this.addClass('over');
}

Drupal.formBuilder.out = function(e, ui) {
  var $this = $(this);
  if (!$this.is('.form-builder-empty-placeholder')) {
    $this.height(0);
  }
  $this.removeClass('over');
};

/**
 * Insert DIVs into empty fieldsets so that items can be dropped within them.
 *
 * This function is called every time an element changes positions during
 * a Sortables drag and drop operation.
 *
 * @param e
 *   The event object containing status information about the event.
 * @param ui
 *   The jQuery Sortables object containing information about the sortable.
 * @param
 */
Drupal.formBuilder.checkFieldsets = function(e, ui, expand) {
  var $fieldsets = $('#form-builder div.form-builder-element fieldset.form-builder-fieldset div.fieldset-wrapper');

  // Find all empty fieldsets.
  $fieldsets.each(function() {
    // Remove placeholders.
    $(this).children('.form-builder-empty-placeholder').remove();
    // If there are no visible children after placeholders are removed, add a placeholder.
    if ($(this).children(':not(.description):visible').length == 0) {
      $(Drupal.settings.formBuilder.emptyFieldset).appendTo(this);
      }
  });
};

Drupal.formBuilder.setActive = function(element, link) {
  Drupal.formBuilder.unsetActive();
  Drupal.formBuilder.activeElement = element;
  Drupal.formBuilder.activeLink = link;
  $(Drupal.formBuilder.activeElement).addClass('form-builder-active');
};

Drupal.formBuilder.unsetActive = function() {
  if (Drupal.formBuilder.activeElement) {
    $(Drupal.formBuilder.activeElement).removeClass('form-builder-active');
    Drupal.formBuilder.activeElement = false;
    Drupal.formBuilder.activeLink = false;
  }
};

Drupal.formBuilder.closeActive = function(callback) {
  if (Drupal.formBuilder.activeElement) {
    var $activeForm = Drupal.formBuilder.fieldConfigureForm ? Drupal.formBuilder.fieldConfigureForm.find('form') : $(Drupal.formBuilder.activeElement).find('form');

    if ($activeForm.length) {
      Drupal.freezeHeight();
      $activeForm.slideUp(function(){
        $(this).remove();
        if (callback && $.isFunction(callback)) {
          callback.call();
        }
      });
    }
  }
  else if (callback) {
    callback.call();
  }

  return false;
};

/**
 * Work around for tabledrags within tabs. On load, if the tab was hidden the
 * offsets cannot be calculated correctly. Recalculate and update the tableDrag.
 */
Drupal.formBuilder.fixTableDragTabs = function(context) {
  if (Drupal.tableDrag && Drupal.tableDrag.length > 1) {
    for (var n in Drupal.tableDrag) {
      if (typeof(Drupal.tableDrag[n]) == 'object') {
        var table = $('#' + n, context).get(0);
        if (table) {
          var indent = Drupal.theme('tableDragIndentation');
          var testCell = $('tr.draggable:first td:first', table).prepend(indent).prepend(indent);
          Drupal.tableDrag[n].indentAmount = $('.indentation', testCell).get(1).offsetLeft - $('.indentation', testCell).get(0).offsetLeft;
          $('.indentation', testCell).slice(0, 2).remove();
        }
      }
    }
  }
};

})(jQuery);
