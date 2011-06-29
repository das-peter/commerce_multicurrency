<?php
/**
 * @file
 * Administrative UI for commerce currency.
 */

/**
 * @param $form
 * @param $form_state
 */
function commerce_currency_conversion_settings_form($form, &$form_state) {
  $enabled_currencies = commerce_currencies(TRUE);
  $conversion_settings = variable_get('commerce_currency_conversion_settings', array());

  $form['conversion_settings'] = array(
    '#attached' => array(
      'js' => array(
        drupal_get_path('module', 'commerce_currency') . '/commerce_currency.admin.js',
      ),
    ),
    '#tree' => TRUE,
  );
  foreach ($enabled_currencies as $currency) {
    if (!isset($conversion_settings[$currency['code']])) {
      $conversion_settings[$currency['code']] = array();
    }
    $conversion_settings[$currency['code']] += array(
      'sync' => '1',
    	'rates' => array(),
    );

    $form['conversion_settings'][$currency['code']] = array(
    	'#type' => 'fieldset',
    	'#collapsible' => TRUE,
    	'#collapsed' => TRUE,
      '#title' => $currency['name'],
    );
    $form['conversion_settings'][$currency['code']]['sync'] = array(
      '#type' => 'checkbox',
      '#title' => t('Autosync all rates of this currency.'),
    	'#states' => array(
        'checked' => array(
        	'#edit-' . $currency['code'] . ' fieldset input[type="checkbox"]' => array('checked' => TRUE),
        ),
      ),
      '#default_value' => $conversion_settings[$currency['code']]['sync'],
    );

    foreach ($enabled_currencies as $conversion_currency) {
      if ($conversion_currency == $currency) {
        continue;
      }
      if (!isset($conversion_settings[$currency['code']]['rates'][$conversion_currency['code']])) {
        $conversion_settings[$currency['code']]['rates'][$conversion_currency['code']] = array();
      }
      $conversion_settings[$currency['code']]['rates'][$conversion_currency['code']] += array(
        'sync' => '1',
      	'rate' => 0,
      );

      $form['conversion_settings'][$currency['code']]['sync']['#states']['checked']['input[name="conversion_settings[' . $currency['code'] . '][rates][' . $conversion_currency['code'] . '][sync]"]'] = array('checked' => TRUE);

      $form['conversion_settings'][$currency['code']]['rates'][$conversion_currency['code']] = array(
      	'#type' => 'fieldset',
      	'#attributes' => array('class' => array('conversion-rates')),
      	'#collapsible' => TRUE,
      	'#collapsed' => FALSE,
      	'#title' => $conversion_currency['code'],
      );
      $form['conversion_settings'][$currency['code']]['rates'][$conversion_currency['code']]['sync'] = array(
        '#type' => 'checkbox',
        '#title' => t('Autosync this conversion rate.'),
      	'#default_value' => $conversion_settings[$currency['code']]['rates'][$conversion_currency['code']]['sync'],
      );
      $form['conversion_settings'][$currency['code']]['rates'][$conversion_currency['code']]['rate'] = array(
        '#type' => 'textfield',
        '#title' => t('Exchange rate'),
      	'#attributes' => array('class' => array('conversion-rate')),
      	'#description' => t(
      		'Exchange rate from @currency_code to @conversion_currency_code.',
      		array('@currency_code' => $currency['name'], '@conversion_currency_code' => $conversion_currency['name'])
    		),
      	'#states' => array(
          'disabled' => array(
            'input[name="conversion_settings[' . $currency['code'] . '][rates][' . $conversion_currency['code'] . '][sync]"]' => array('checked' => TRUE),
          ),
        ),
      	'#size' => 13,
      	'#maxlength' => 20,
      	'#number_type' => 'decimal',
      	'#element_validate' => array('commerce_currency_conversion_settings_form_rate_validate'),
      	'#default_value' => $conversion_settings[$currency['code']]['rates'][$conversion_currency['code']]['rate'],
      	'#field_suffix' => t(
      		'* 100 @currency_symbol = !amount @conversion_currency_symbol',
    		  array(
    		  	'@currency_symbol' => $currency['symbol'],
            '@conversion_currency_symbol' => $conversion_currency['symbol'],
            '!amount' => '<span class="amount">' . (100 * $conversion_settings[$currency['code']]['rates'][$conversion_currency['code']]['rate']) . '</span>',
  		  	)
    		),
      );
    }
  }

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
  );

  return $form;
}

/**
 * Validate a rate.
 *
 * Heavy borrowed from number module
 * @see number_field_widget_validate()
 *
 * @param $form
 * @param $form_state
 */
function commerce_currency_conversion_settings_form_rate_validate($element, &$form_state) {
  $value = $element['#value'];
  // Reject invalid characters.
  if (!empty($value)) {
    $regexp = '@[^0-9\\.]@';
    $message = t('Only numbers and the decimal separator (@separator) allowed.', array('@separator' => '.'));
    if ($value != preg_replace($regexp, '', $value)) {
      form_error($element, $message);
    }
  }
}

/**
 * @param $form
 * @param $form_state
 */
function commerce_currency_conversion_settings_form_submit($form, &$form_state) {
  variable_set('commerce_currency_conversion_settings', $form_state['values']['conversion_settings']);
}
