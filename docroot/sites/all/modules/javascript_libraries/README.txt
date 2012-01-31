
The purpose of the JavaScript Libraries Manager is to make certain JavaScript
libraries shipped with Drupal core available so that they can be combined
with user-uploaded or external JavaScript files or libraries.

This module requires the 'administer site configuration' permission in order to
use it.

IMPORTANT: Any uploaded file or external file included can compromise you
site's security, or that of the sites visitors. Do not upload file or
include external files unles you are certain they are safe.

IMPORTANT: Including external JavaScript files may cause your site to appear
slow.  In general, only include links to well-trusted public files hosted
on a CDN or high availability server.

An example script to start with as an uploaded file:

jQuery(document).ready(function ($) {
  $('p').css( "background", "red" );
});            

/*****************************************************************
 *
 * Simple jQuery UI Tabs example
 *
 *****************************************************************/

1. Enabled the jQuery UI: Tabs library 
at /admin/config/system/javascript-libraries

2. Create a node or place the following HTML code in a template:
@see http://jqueryui.com/demos/tabs/

<div class="custom-tabs">
	<ul>
		<li><a href="#tabs-1">Tab 1</a></li>
		<li><a href="#tabs-2">Tab 2</a></li>
		<li><a href="#tabs-3">Tab 3</a></li>
	</ul>
	<div id="tabs-1">
	  <p>Content of tab 1</p>
	</div>
	<div id="tabs-2">
	  <p>Content of tab 2</p>
	</div>
	<div id="tabs-3">
	  <p>Content of tab 3</p>
	</div>
</div>

3. Place the following code in a new JavaScript file:

jQuery(document).ready(function($) {
  if ($.isFunction($.fn.tabs)) {
    $('.custom-tabs').tabs();
  }
});

4. Upload this new JavaScript file to the Footer region under the JavaScript
Libraries module Custom tab at
/admin/config/system/javascript-libraries/custom

5. Refresh the page with your custom HTML (see 2. above). Your HTML should now
render as a jQuery UI tab element.
