<?php

/**
 * @file
 * Hooks provided by the Simplified Modules module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Define modules that will be auto-enabled when all their dependencies are met.
 *
 * These modules are typically part of a package that contains many submodules,
 * but on certain sites you just want to have one choice on the modules page,
 * with the submodules turned on automatically whenever they are able to be.
 *
 * Any modules which you return from this hook will be hidden from the modules
 * page, and turned on automatically whenever all their dependencies are met.
 * (They will also be turned off automatically when their dependencies are no
 * longer met.)
 *
 * @return
 *   An array of module names.
 *
 * @see hook_simplified_modules_hidden_submodules_alter()
 */
function hook_simplified_modules_hidden_submodules() {
  return array(
    // Prevent the Views UI module from appearing on the Modules page, but turn
    // it on automatically whenever Views (which it depends on) is enabled.
    'views_ui',

    // Prevent a number of the component modules from the XML Sitemap package
    // from appearing on the Modules page, and enable their functionality
    // automatically whenever available. (For example, the 'xmlsitemap_menu'
    // module will automatically turn on whenever XML Sitemap and Menu are both
    // enabled, since it depends on both of them.)
    'xmlsitemap_engines',
    'xmlsitemap_menu',
    'xmlsitemap_node',
    'xmlsitemap_taxonomy',

    // Note that in general you can determine dependencies from looking at a
    // module's info file.
    //
    // For the modules discussed above, views_ui.info has this line:
    //   dependencies[] = views
    //
    // And xmlsitemap_menu.info has these lines:
    //   dependencies[] = xmlsitemap
    //   dependencies[] = menu
  );
}

/**
 * Alter the modules that will be auto-enabled when all their dependencies are met.
 *
 * @param $modules
 *   An array of module names returned from implementations of
 *   hook_simplified_modules_hidden_submodules().
 *
 * @see hook_simplified_modules_hidden_submodules()
 */
function hook_simplified_modules_hidden_submodules_alter(&$modules) {
  // Remove any occurrences of 'xmlsitemap_menu' from the list and make it go
  // back to appearing on the modules page like it normally would.
  $modules = array_diff($modules, array('xmlsitemap_menu'));
}

/**
 * Define modules that will be auto-enabled whenever any dependents modules are turned on.
 *
 * These are typically modules which are required for a major site feature to
 * work correctly, but which might be confusing if exposed on the modules page
 * (for example, an API-only module).
 *
 * Any modules which you return from this hook will be hidden from the modules
 * page and turned on automatically whenever another module that depends on
 * them is enabled. (They will also be turned off automatically when they are
 * no longer needed.)
 *
 * @return
 *   An array of module names.
 *
 * @see hook_simplified_modules_hidden_dependencies_alter()
 */
function hook_simplified_modules_hidden_dependencies() {
  return array(
    // Prevent the Date API module from appearing on the Modules page, but turn
    // it on automatically whenever any date-related module that depends on it
    // is enabled.
    'date_api',

    // The modules listed below are the full list of requirements for the
    // Webform Alternate UI module (an integration module which provides an
    // alternate interface for editing webforms). By listing them here, they
    // will all be hidden from the Modules page, and when the user enables
    // Webform Alternate UI, all its requirements will automatically be turned
    // on in one click.
    'webform',
    'form_builder',
    'form_builder_webform',
    'options_element',
    'ux_elements',
  );
}

/**
 * Alter the modules that will be auto-enabled whenever any dependents modules are turned on.
 *
 * @param $modules
 *   An array of module names returned from implementations of
 *   hook_simplified_modules_hidden_dependencies().
 *
 * @see hook_simplified_modules_hidden_dependencies()
 */
function hook_simplified_modules_hidden_dependencies_alter(&$modules) {
  // Remove any occurrences of 'webform' from the list and make the Webform
  // module go back to appearing on the modules page like it normally would.
  $modules = array_diff($modules, array('webform'));
}

/**
 * @} End of "addtogroup hooks".
 */
