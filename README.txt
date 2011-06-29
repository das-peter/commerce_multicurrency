
-- SUMMARY --

This module enhances the multi currency capabilities of commerce
http://drupal.org/project/commerce


-- REQUIREMENTS --

Commerce: http://drupal.org/project/commerce


-- INSTALLATION --

* Install as usual, see http://drupal.org/node/70151 for further information.

* Configure the currency covnersion: admin/commerce/config/currency/conversion

* Run cron or sync manually to syncronize the rates.



-- API --
It is possible to add own functions / services to sync the conversion rates.
Check commerce_currency.api.php and commerce_currency.ecb.inc for further 
information and examples.

Currently the sync is triggered manually or by cron.