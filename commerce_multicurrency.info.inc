<?php

/**
 * @file
 * Provides metadata for the shopping cart order.
 */

/**
 * Implements hook_entity_property_info_alter().
 */
function commerce_multicurrency_entity_property_info_alter(&$info) {
  // Add the current user's currency to the site information.
  $info['site']['properties']['commerce_currency'] = array(
    'label' => t("User's currency"),
    'description' => t('The currency to use for the current user.'),
    'getter callback' => 'commerce_multicurrency_get_properties',
    'type' => 'text',
  );
}
