DESCRIPTION
-----------

This is a simple module which forces all your site's webforms and webform
submission results to be viewed and submitted entirely via SSL (i.e., at a URL
beginning with https://).

CAVEATS
-------

Although the module should work well in most scenarios (and with standard uses
of the Webform module), be warned that it cannot deal with all possible
customizations you may have made on your site and all possible places where
webform submission results can theoretically be viewed.

For example, if your site has custom Views which display webform submissions,
this module is not able to automatically detect those and protect them via SSL
(instead, you will need to configure the Secure Pages module to specifically
protect the pages on which those Views are displayed).

Also, files which are uploaded along with a webform submission will not
necessarily be transferred via SSL when downloaded by an administrator (even
for private files which Drupal otherwise controls).

In general, it is important to remember that although this module makes a "best
effort" to protect your webform submissions via SSL, you are still responsible
for auditing the behavior of your site if you have policies in place that
require all user submissions to be handled securely.

INSTALLATION
------------

1. Download the Secure Pages module (http://drupal.org/project/securepages) in
   addition to this one, and place it in your site's normal modules directory
   (e.g., sites/all/modules).

2. Enable the Webform SSL module as you would any other, via the Modules page
   at admin/modules.

3. Make sure your web server is configured to support SSL, and configure Drupal
   to support SSL also, as described at http://drupal.org/project/securepages
   (by setting "$conf['https'] = TRUE;" in your settings.php file or otherwise
   setting the 'https' variable directly in the database, e.g. using Drush).

4. Go to admin/config/system/securepages and toggle the "Enable Secure Pages"
   setting on.

5. Adjust any other settings at admin/config/system/securepages that you need
   to. Note that the Webform SSL module does not have any settings of its own;
   it will automatically protect all webforms and webform submission results
   via SSL when they are being viewed or submitted. So it is not actually
   necessary to have anything configured under the "Pages" setting at
   admin/config/system/securepages in order for this module to work. However,
   you may want to leave some or all of the default "Pages" settings if you
   would also like other pages on your site to be secure (for example, "user"
   and "user/*" to secure the login pages).

6. Download the Webform module (http://drupal.org/project/webform), enable it,
   and start creating webforms. They will automatically be protected via SSL.

MAINTAINERS
-----------

This module was contributed by Acquia and is currently maintained by the Acquia
engineering team.
