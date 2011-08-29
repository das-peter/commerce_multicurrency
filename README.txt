
-- SUMMARY --

This module enhances some of the multi-currency capabilities of commerce
http://drupal.org/project/commerce


-- REQUIREMENTS --

Commerce: http://drupal.org/project/commerce


-- INSTALLATION --

* Install as usual, see http://drupal.org/node/70151 for further information.

* Configure the currency conversion: admin/commerce/config/currency/conversion

* Run cron or sync manually to synchronize the rates.



-- API --
It is possible to add own functions / services to sync the conversion rates.
Check commerce_multicurrency.api.php and commerce_multicurrency.ecb.inc for further
information and examples.

The sync is triggered manually or by cron.