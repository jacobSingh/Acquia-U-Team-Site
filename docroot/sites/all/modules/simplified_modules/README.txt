HOW TO USE THIS MODULE
----------------------

This module provides an API to allow related modules on the Modules page to be
grouped under a single checkbox.

If your site's Modules page is growing uncontrollably in length and complexity,
this module can help you simplify it. You can use the API to hide less
important modules from the page, and make it so those modules are instead
turned on and off automatically behind the scenes, exactly when they are
needed.

For example, you might use the API to declare that all the modules that ship
with the XML Sitemap project should appear as a single checkbox labeled "XML
"Sitemap" on the modules page (with the component modules automatically enabled
behind the scenes whenever they can be).

Or, you might group the Views and Views UI module under a single checkbox
labeled "Views". Or hide the Date API module, but turn it on automatically
whenever any date-related module that depends on it is enabled.

See the simplified_modules.api.php file included in this directory for more
information about the API and available hooks.

REQUIRED PATCHES
----------------

Until the Drupal core bug at http://drupal.org/node/1205684 is fixed, you will
likely want to apply the patch in that issue to your installation if you are
using this module.

This will avoid hidden modules unexpectedly appearing in a confirmation form
every time the form on the Modules page is submitted.

WHAT IF I'M NOT A DEVELOPER?
----------------------------

Currently, this module does not do much on its own; it's primarily an API
module for developers to use to simplify the Modules page (e.g. on particular
sites).

When the module is installed, it does cause all non-core modules on the Modules
page to be listed together in the "Other" category, rather than scattered among
different packages. But generally, it will only be useful if you've also
implemented the API to customize the Modules page experience for the site you
are building.

MAINTAINERS
-----------

This module was contributed by Acquia as part of the Drupal Gardens project and
is currently maintained by the Acquia design and engineering teams.
