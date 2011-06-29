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
  $sync_provider = variable_get('commerce_currency_sync_provider', NULL);

  $sync_providers = array();
  foreach (commerce_currency_commerce_currency_exchange_rate_sync_provider() as $provider_code => $provider) {
    $sync_providers[$provider_code] = $provider['title'];
  }
  $form['sync_provider'] = array(
    '#type' => 'select',
    '#options' => $sync_providers,
    '#default_value' => $sync_provider,
    '#title' => t('Service to fetch exchange rates from:'),
  );
  $form['sync_now'] = array(
    '#type' => 'checkbox',
    '#title' => t('Synchronize rates on save.'),
  );
  $form['demo_amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount for example conversions:'),
  	'#size' => 5,
    '#default_value' => 100,
  );

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
      '#title' => t('Synchronize all rates of this currency.'),
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
        '#title' => t('Synchronize this conversion rate.'),
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
        '#element_validate' => array('commerce_currency_conversion_settings_form_rate_validate'),
        '#default_value' => $conversion_settings[$currency['code']]['rates'][$conversion_currency['code']]['rate'],
        '#field_suffix' => t(
          '* !demo_amount @currency_symbol = !amount @conversion_currency_symbol',
          array(
      			'!demo_amount' => '<span class="demo-amount">100</span>',
            '@currency_symbol' => $currency['symbol'],
            '@conversion_currency_symbol' => $conversion_currency['symbol'],
            '!amount' => '<span class="demo-amount-converted">' . (100 * $conversion_settings[$currency['code']]['rates'][$conversion_currency['code']]['rate']) . '</span>',
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
 * Heavy borrowed from number module:
 * @see number_field_widget_validate()
 *
 * @param $form
 * @param $form_state
 */
function commerce_currency_conversion_settings_form_rate_validate($element, &$form_state) {
  $value = trim($element['#value']);
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
 * Submit handler for the settings form.
 * @param $form
 * @param $form_state
 */
function commerce_currency_conversion_settings_form_submit($form, &$form_state) {
  variable_set('commerce_currency_conversion_settings', $form_state['values']['conversion_settings']);
  variable_get('commerce_currency_sync_provider', $form_state['values']['sync_provider']);
  if (!empty($form_state['values']['sync_now'])) {
    commerce_currency_sync_exchange_rates_now();
  }
}
