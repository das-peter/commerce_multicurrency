<?php

/**
 * @file
 * Defines default currency selection components and rules.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_currency_default_rules_configuration() {
  $rules = array();

  $reaction_rule = rules_reaction_rule();
  $reaction_rule->label = t('Set the currency price');
  $reaction_rule->active = TRUE;
  $reaction_rule->weight = -9;

    foreach (commerce_currencies(TRUE) as $currency_code => $currency_info) {
      $field_name = 'commerce_price_' . strtolower($currency_code);
      $rule_name = 'commerce_currency_use_' . strtolower($currency_code) . '_price';
      $field_info = field_info_field($field_name);
      if (!$field_info){
        $saved_rule = rules_config_load($rule_name);
        if ($saved_rule !== FALSE && ($saved_rule->status | ENTITY_IN_CODE)) {
          $saved_rule->delete();
        }
        continue;
      }
      $rule = rule(array(
        'commerce_line_item' => array(
          'type' => 'commerce_line_item',
          'label' => t('Line item'),
        ),
      ));
      $rule->label = t('Use the ' . $currency_code . ' price');
      $rule->active = TRUE;
      $rule
        ->condition('data_is', array(
          'data:select' => 'site:commerce-currency',
          'value' => $currency_code,
        ))
        ->condition('entity_has_field', array(
          'entity:select' => 'commerce-line-item',
          'field' => 'commerce_product',
        ))
        ->condition('entity_has_field', array(
          'entity:select' => 'commerce-line-item:commerce-product',
          'field' => $field_name,
        ))
        ->action('data_set', array(
          'data:select' => 'commerce-line-item:commerce-unit-price',
          'value:select' => 'commerce-line-item:commerce_product:' . $field_name,
        ));
      $rules[$rule_name] = $rule;
      $reaction_rule->action('component_' . $rule_name, array(
        'commerce_line_item:select' => 'commerce-line-item',
      ));
    }

    $reaction_rule
      ->event('commerce_product_calculate_sell_price');
    $rules['commerce_currency_set_currency_price'] = $reaction_rule;

  return $rules;
}

