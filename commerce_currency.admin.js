(function ($) {
  $(document).ready(function() {
    $('.conversion-rates input.conversion-rate').keyup(function(){
      var rate = $(this).val();
      if ($(this).val().search(',')) {
        $(this).val($(this).val().replace(',', '.'));
      }
      var example = '--';
      if (Number(rate)) {
        example = rate * 100;
      }
      $(this).parent().find('.amount').html(example);
    });
  });
})(jQuery);